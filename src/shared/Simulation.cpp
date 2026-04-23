#include "shared/Simulation.hpp"

#include <cmath>
#include <iostream>

#include "core/ICollectible.hpp"

// Helper: compute pixel offsets from grid/window dimensions
static void computeOffsets(const LevelManager& level, float scaledTileSize, float& offX, float& offY) {
    offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;
}

Simulation::Simulation() {
    ghosts[0] = std::make_unique<Ghost>(std::make_unique<BlinkyAI>());
    ghosts[1] = std::make_unique<Ghost>(std::make_unique<PinkyAI>());
    ghosts[2] = std::make_unique<Ghost>(std::make_unique<InkyAI>());
    ghosts[3] = std::make_unique<Ghost>(std::make_unique<ClydeAI>());
}

void Simulation::initLevel(int levelNumber) {
    currentLevel_ = levelNumber;
    level.loadLevel(levelNumber);
    initializedPositions    = false;
    initialStartingPellets_ = level.getRemainingPellets();
    fruit70Processed_       = false;
    fruit170Processed_      = false;
}

void Simulation::resetForNewMatch(int levelNumber) {
    players[0] = Player(0);
    players[1] = Player(1);
    ghosts[0]  = std::make_unique<Ghost>(std::make_unique<BlinkyAI>());
    ghosts[1]  = std::make_unique<Ghost>(std::make_unique<PinkyAI>());
    ghosts[2]  = std::make_unique<Ghost>(std::make_unique<InkyAI>());
    ghosts[3]  = std::make_unique<Ghost>(std::make_unique<ClydeAI>());

    initializedPositions = false;
    ghostHomeX = ghostHomeY = houseCenterX = houseCenterY = houseExitX = houseExitY = 0.f;
    for (int i = 0; i < 4; ++i) {
        ghostState[i]        = GhostState::InHouse;
        ghostReleaseTimer[i] = 0.5f + i * 1.0f;
        ghostRespawn[i]      = 0.f;
    }
    consumedThisTick.clear();
    eatenGhostsThisTick.clear();
    gridTileUpdatesThisTick.clear();
    fruitPopupsThisTick.clear();
    {
        std::random_device rd;
        bonusFruitRng_.seed(rd());
    }
    ghostMode  = GhostMode::Scatter;
    phaseIndex = 0;
    phaseTimer = PHASE_SCHEDULE[0];
    initLevel(levelNumber);
}

void Simulation::setDesired(int playerIndex, Direction d) {
    if (playerIndex < 0 || playerIndex > 1) return;
    players[playerIndex].getPacman().setDesired(d);
}

void Simulation::step(float dt, float scaledTileSize, float scale) {
    if (!initializedPositions) {
        float offX, offY;
        computeOffsets(level, scaledTileSize, offX, offY);
        const int gridW = level.getWidth();
        const int gridH = level.getHeight();

        // Spawn P0 at first pellet in row 1, P1 at last
        auto placePlayer = [&](int pi, bool fromLeft) {
            bool placed = false;
            if (gridH > 1) {
                int start = fromLeft ? 0 : gridW - 1;
                int end   = fromLeft ? gridW : -1;
                int step  = fromLeft ? 1 : -1;
                for (int x = start; x != end && !placed; x += step) {
                    if (level.getTile(x, 1) == TileType::Pellet) {
                        players[pi].getPacman().setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                                            offY + 1 * scaledTileSize + scaledTileSize / 2.f);
                        placed = true;
                    }
                }
            }
            if (!placed) {
                for (int y = 0; y < gridH && !placed; ++y) {
                    int xStart = fromLeft ? 0 : gridW - 1;
                    int xEnd   = fromLeft ? gridW : -1;
                    int xStep  = fromLeft ? 1 : -1;
                    for (int x = xStart; x != xEnd && !placed; x += xStep) {
                        if (level.getTile(x, y) != TileType::Wall) {
                            players[pi].getPacman().setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                                                offY + y * scaledTileSize + scaledTileSize / 2.f);
                            placed = true;
                        }
                    }
                }
            }
            auto pos = players[pi].getPacman().getPosition();
            players[pi].setSpawn(pos.x, pos.y);
        };

        placePlayer(0, true);
        placePlayer(1, false);

        // Place ghosts at house center
        float gx     = offX + (gridW / 2) * scaledTileSize + scaledTileSize * 0.5f;
        float gy     = offY + (gridH / 2) * scaledTileSize + scaledTileSize * 0.5f;
        houseCenterX = gx;
        houseCenterY = gy;

        // Find exit above the house
        int exitGX = gridW / 2;
        int exitGY = gridH / 2;
        while (exitGY > 0 && level.getTile(exitGX, exitGY) == TileType::Empty) --exitGY;
        while (exitGY > 0 && level.getTile(exitGX, exitGY) == TileType::Wall) --exitGY;
        houseExitX = offX + exitGX * scaledTileSize + scaledTileSize * 0.5f;
        houseExitY = offY + exitGY * scaledTileSize + scaledTileSize * 0.5f;

        ghosts[0]->setPosition(houseCenterX - scaledTileSize * 0.5f, houseCenterY);
        ghosts[1]->setPosition(houseCenterX + scaledTileSize * 0.5f, houseCenterY);
        ghosts[2]->setPosition(houseCenterX, houseCenterY + scaledTileSize * 0.5f);
        ghosts[3]->setPosition(houseCenterX, houseCenterY - scaledTileSize * 0.5f);
        ghostHomeX = houseCenterX;
        ghostHomeY = houseCenterY;

        // Stagger release timers
        for (int i = 0; i < 4; ++i) {
            ghostReleaseTimer[i] = 0.5f + i * 1.0f;
            ghostState[i]        = GhostState::InHouse;
        }

        initializedPositions = true;
    }

    // --- Resolve lethal collisions ---
    float colRadius = scaledTileSize * 0.5f * 0.8f;
    InteractionResolver::resolveLethal(players, ghosts, colRadius);

    // --- Update player respawns ---
    updatePlayerRespawns(dt);

    // --- Update Pacman logic ---
    for (int pi = 0; pi < 2; ++pi) {
        if (!players[pi].isDying()) players[pi].getPacman().update(dt, level, scaledTileSize, scale);
    }

    // --- Update ghosts ---
    auto p0    = players[0].getPacman().getPosition();
    auto p1    = players[1].getPacman().getPosition();
    auto p0dir = players[0].getPacman().getFacing();

    for (int gi = 0; gi < 4; ++gi) {
        Ghost& g = *ghosts[gi];
        if (ghostRespawn[gi] > 0.f) {
            g.setFrightened(0.f);
            g.setPosition(houseCenterX, houseCenterY);
            continue;
        }
        if (ghostState[gi] == GhostState::InHouse) {
            ghostReleaseTimer[gi] -= dt;
            if (ghostReleaseTimer[gi] <= 0.f) ghostState[gi] = GhostState::Exiting;
        }
        if (ghostState[gi] == GhostState::Exiting) {
            updateGhostExiting(gi, dt, scaledTileSize, scale);
        }
        if (ghostState[gi] == GhostState::Roaming) {
            float elroy = 1.f;
            if (gi == 0 && ghostMode == GhostMode::Chase && !g.isFrightened()) {
                // Cruise Elroy: Blinky accelerates in chase as the maze is cleared (two tiers).
                const int pel = level.getRemainingPellets();
                if (pel <= 10)
                    elroy = 1.12f;
                else if (pel <= 20)
                    elroy = 1.08f;
            }
            g.updateLogic(dt, level, scaledTileSize, scale, p0, p0dir, p1, ghostMode, elroy);
        }
    }

    // --- Scatter/Chase mode cycling ---
    updateGhostMode(dt);

    // --- Pellet collection ---
    consumedThisTick.clear();
    float offX, offY;
    computeOffsets(level, scaledTileSize, offX, offY);

    for (int pi = 0; pi < 2; ++pi) {
        if (players[pi].isDying()) continue;
        auto p    = players[pi].getPacman().getPosition();
        int  curX = static_cast<int>(std::floor((p.x - offX) / scaledTileSize));
        int  curY = static_cast<int>(std::floor((p.y - offY) / scaledTileSize));

        const ICollectible* c = level.collectAt(curX, curY);
        if (c) {
            players[pi].addScore(c->points());
            const float panic = c->frightenedModeDuration();
            if (panic > 0.f) {
                players[pi].setPowerTimer(panic);
                players[pi].resetFrightenedEatCount();
                for (auto& gp : ghosts) gp->setFrightened(panic);
            }
            if (!c->countsTowardLevelPelletTotal() && c->frightenedModeDuration() <= 0.f) {
                const float fcx = offX + curX * scaledTileSize + scaledTileSize * 0.5f;
                const float fcy = offY + curY * scaledTileSize + scaledTileSize * 0.5f;
                fruitPopupsThisTick.push({fcx, fcy, c->points()});
            }
            consumedThisTick.push({curX, curY, c});
        }

        players[pi].tickPowerTimer(dt);
    }

    updateFruitSpawns();

    // --- Resolve frightened collisions (ghost eating) ---
    std::vector<GhostEatenEvent> eatenEvents;
    InteractionResolver::resolveFrightened(players, ghosts, colRadius, ghostHomeX, ghostHomeY, eatenEvents,
                                           ghostRespawn);
    for (auto& ev : eatenEvents) {
        eatenGhostsThisTick.push({ev.x, ev.y, ev.points});
    }

    // --- Update ghost respawn timers ---
    updateGhostRespawns(dt, scaledTileSize, scale);
}

void Simulation::updateGhostExiting(int idx, float dt, float scaledTileSize, float scale) {
    Ghost& ghost = *ghosts[idx];
    Vec2   pos   = ghost.getPosition();

    // Lock to exit column
    if (std::abs(pos.x - houseExitX) > 0.01f) {
        ghost.setPosition(houseExitX, pos.y);
        pos = ghost.getPosition();
    }

    float dy = houseExitY - pos.y;
    if (std::abs(dy) < scaledTileSize * 0.25f) {
        // Snap to tile and transition to Roaming
        float offX, offY;
        computeOffsets(level, scaledTileSize, offX, offY);
        int   curX = static_cast<int>(std::floor((pos.x - offX) / scaledTileSize));
        int   curY = static_cast<int>(std::floor((pos.y - offY) / scaledTileSize));
        float cx   = offX + curX * scaledTileSize + scaledTileSize * 0.5f;
        float cy   = offY + curY * scaledTileSize + scaledTileSize * 0.5f;
        ghost.setPosition(cx, cy);
        ghostState[idx] = GhostState::Roaming;
    } else {
        float speed = 64.f * 6.f * scale;
        float step  = std::copysign(speed * dt, dy);
        if (std::abs(step) > std::abs(dy)) step = dy;
        ghost.setPosition(pos.x, pos.y + step);
    }
}

PlayerStateView Simulation::getPlayerState(int playerIndex) const {
    PlayerStateView v{};
    if (playerIndex < 0 || playerIndex > 1) return v;
    const Player& p = players[playerIndex];
    v.position      = p.getPacman().getPosition();
    v.facing        = p.getPacman().getFacing();
    v.score         = p.getScore();
    v.powered       = p.isPowered();
    v.powerTimeLeft = p.getPowerTimer();
    v.livesLeft     = p.getLives();
    v.deathTimeLeft = p.getDeathTimer();
    return v;
}

void Simulation::drainConsumed(std::vector<ConsumedPellet>& out) {
    consumedThisTick.drainTo(out);
}

void Simulation::drainEatenGhosts(std::vector<EatenGhostEvent>& out) {
    eatenGhostsThisTick.drainTo(out);
}

void Simulation::drainGridTileUpdates(std::vector<GridTileUpdate>& out) {
    gridTileUpdatesThisTick.drainTo(out);
}

void Simulation::drainFruitPopups(std::vector<EatenGhostEvent>& out) {
    fruitPopupsThisTick.drainTo(out);
}

void Simulation::updateFruitSpawns() {
    // Classic schedule: first bonus after 70 pellets, second after 170 (cumulative) eaten this level.
    // Spawn cells match CLASSIC_MAZE: two empty tunnel slots at (9,21) and (15,21) — not row 20 (pellet/wall).
    static constexpr int               kFirstMilestone  = 70;
    static constexpr int               kSecondMilestone = 170;
    static constexpr int               kFruitTunnelRowY = 21;
    const int                          eaten            = initialStartingPellets_ - level.getRemainingPellets();
    std::uniform_int_distribution<int> tierDist(0, 3);

    if (!fruit70Processed_ && eaten >= kFirstMilestone) {
        const int      tier  = tierDist(bonusFruitRng_);
        const TileType fruit = static_cast<TileType>(static_cast<int>(TileType::BonusFruit1) + tier);
        const int      x = 9, y = kFruitTunnelRowY;
        if (level.getTile(x, y) == TileType::Empty) {
            fruit70Processed_ = true;
            level.setTile(x, y, fruit);
            gridTileUpdatesThisTick.push({static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                                          static_cast<std::uint8_t>(static_cast<int>(fruit))});
        }
    }
    if (!fruit170Processed_ && eaten >= kSecondMilestone) {
        const int      tier  = tierDist(bonusFruitRng_);
        const TileType fruit = static_cast<TileType>(static_cast<int>(TileType::BonusFruit1) + tier);
        const int      x = 15, y = kFruitTunnelRowY;
        if (level.getTile(x, y) == TileType::Empty) {
            fruit170Processed_ = true;
            level.setTile(x, y, fruit);
            gridTileUpdatesThisTick.push({static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                                          static_cast<std::uint8_t>(static_cast<int>(fruit))});
        }
    }
}

Vec2 Simulation::getGhostPosition(int ghostIndex) const {
    if (ghostIndex < 0 || ghostIndex > 3) return Vec2{};
    return ghosts[ghostIndex]->getPosition();
}

Direction Simulation::getGhostFacing(int ghostIndex) const {
    if (ghostIndex < 0 || ghostIndex > 3) return Direction::None;
    return ghosts[ghostIndex]->getFacing();
}

bool Simulation::isGhostActive(int ghostIndex) const {
    if (ghostIndex < 0 || ghostIndex > 3) return false;
    return ghostRespawn[ghostIndex] <= 0.f;
}

bool Simulation::isGhostFrightened(int ghostIndex) const {
    if (ghostIndex < 0 || ghostIndex > 3) return false;
    return ghostRespawn[ghostIndex] <= 0.f && ghosts[ghostIndex]->isFrightened();
}

void Simulation::updatePlayerRespawns(float dt) {
    for (int i = 0; i < 2; ++i) {
        bool wasDying = players[i].isDying();
        players[i].tickDeathTimer(dt);
        // After death timer elapses, reset position to spawn
        if (wasDying && !players[i].isDying()) {
            players[i].getPacman().setPosition(players[i].getSpawnX(), players[i].getSpawnY());
            players[i].getPacman().resetDirection();
        }
    }
}

void Simulation::updateGhostRespawns(float dt, float /*scaledTileSize*/, float /*scale*/) {
    for (int i = 0; i < 4; ++i) {
        if (ghostRespawn[i] > 0.f) {
            ghostRespawn[i] -= dt;
            if (ghostRespawn[i] < 0.f) ghostRespawn[i] = 0.f;
            if (ghostRespawn[i] == 0.f) {
                ghostState[i]        = GhostState::InHouse;
                ghostReleaseTimer[i] = 1.0f + 0.5f * i;
            }
        }
    }
}

bool Simulation::isMatchLost() const {
    for (int i = 0; i < 2; ++i) {
        if (!players[i].isAlive()) return true;
    }
    return false;
}

bool Simulation::isLevelCleared() const {
    return level.getRemainingPellets() == 0;
}

void Simulation::updateGhostMode(float dt) {
    // Once all phases are exhausted the ghosts stay in Chase permanently.
    if (phaseIndex >= PHASE_COUNT) return;

    phaseTimer -= dt;
    if (phaseTimer > 0.f) return;

    // Advance to the next phase.
    ++phaseIndex;
    GhostMode newMode = (phaseIndex % 2 == 0) ? GhostMode::Scatter : GhostMode::Chase;

    // Carry any overshoot into the next phase.
    if (phaseIndex < PHASE_COUNT) {
        phaseTimer = PHASE_SCHEDULE[phaseIndex] + phaseTimer;  // phaseTimer is negative here
    }

    // Only reverse and switch mode if it actually changed.
    if (newMode == ghostMode) return;
    ghostMode = newMode;

    // Ghosts that are roaming and not frightened immediately reverse.
    for (int i = 0; i < 4; ++i) {
        if (ghostState[i] == GhostState::Roaming && ghostRespawn[i] <= 0.f && !ghosts[i]->isFrightened()) {
            ghosts[i]->reverseDirection();
        }
    }
}
