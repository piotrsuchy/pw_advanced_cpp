#include "shared/Simulation.hpp"

#include <cmath>
#include <iostream>

Simulation::Simulation() {}

void Simulation::initLevel(int levelNumber) {
    level.loadLevel(levelNumber);
    initializedPositions = false;
}

void Simulation::setDesired(int playerIndex, Direction d) {
    if (playerIndex < 0 || playerIndex > 1) return;
    players[playerIndex].setDesired(d);
}

void Simulation::step(float dt, float scaledTileSize, float scale) {
    if (!initializedPositions) {
        // Compute offsets as in rendering to align world/grid coordinates
        const int   gridW = level.getWidth();
        const int   gridH = level.getHeight();
        const float offX  = (800.f - gridW * scaledTileSize) / 2.f;
        const float offY  = (600.f - gridH * scaledTileSize) / 2.f;

        // Spawn P0 at first pellet in row 1 (left side), fallback to any non-wall
        bool p0Placed = false;
        if (gridH > 1) {
            for (int x = 0; x < gridW; ++x) {
                if (level.getTile(x, 1) == TileType::Pellet) {
                    players[0].setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                           offY + 1 * scaledTileSize + scaledTileSize / 2.f);
                    p0Placed = true;
                    break;
                }
            }
        }
        if (!p0Placed) {
            for (int y = 0; y < gridH && !p0Placed; ++y) {
                for (int x = 0; x < gridW; ++x) {
                    if (level.getTile(x, y) != TileType::Wall) {
                        players[0].setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                               offY + y * scaledTileSize + scaledTileSize / 2.f);
                        p0Placed = true;
                        break;
                    }
                }
            }
        }

        // Spawn P1 at right-most pellet in row 1, fallback to any non-wall from right/bottom
        bool p1Placed = false;
        if (gridH > 1) {
            for (int x = gridW - 1; x >= 0; --x) {
                if (level.getTile(x, 1) == TileType::Pellet) {
                    players[1].setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                           offY + 1 * scaledTileSize + scaledTileSize / 2.f);
                    p1Placed = true;
                    break;
                }
            }
        }
        if (!p1Placed) {
            for (int y = gridH - 1; y >= 0 && !p1Placed; --y) {
                for (int x = gridW - 1; x >= 0; --x) {
                    if (level.getTile(x, y) != TileType::Wall) {
                        players[1].setPosition(offX + x * scaledTileSize + scaledTileSize / 2.f,
                                               offY + y * scaledTileSize + scaledTileSize / 2.f);
                        p1Placed = true;
                        break;
                    }
                }
            }
        }

        // Store spawn positions for players
        spawnX[0] = players[0].getPosition().x;
        spawnY[0] = players[0].getPosition().y;
        spawnX[1] = players[1].getPosition().x;
        spawnY[1] = players[1].getPosition().y;

        // Place ghosts near the center box corners
        float gx = offX + (gridW / 2) * scaledTileSize + scaledTileSize * 0.5f;
        float gy = offY + (gridH / 2) * scaledTileSize + scaledTileSize * 0.5f;
        // Define house center (middle empty area) and compute a real exit tile just outside the house
        houseCenterX = gx;
        houseCenterY = gy;
        // Find exit above the house: walk up from center through Empty, then through Wall, then stop at first non-Wall
        int exitGX = gridW / 2;
        int exitGY = gridH / 2;
        while (exitGY > 0 && level.getTile(exitGX, exitGY) == TileType::Empty) {
            --exitGY;
        }
        while (exitGY > 0 && level.getTile(exitGX, exitGY) == TileType::Wall) {
            --exitGY;
        }
        houseExitX = offX + exitGX * scaledTileSize + scaledTileSize * 0.5f;
        houseExitY = offY + exitGY * scaledTileSize + scaledTileSize * 0.5f;

        blinky.setPosition(houseCenterX - scaledTileSize * 0.5f, houseCenterY);
        pinky.setPosition(houseCenterX + scaledTileSize * 0.5f, houseCenterY);
        inky.setPosition(houseCenterX, houseCenterY + scaledTileSize * 0.5f);
        clyde.setPosition(houseCenterX, houseCenterY - scaledTileSize * 0.5f);
        ghostHomeX = houseCenterX;
        ghostHomeY = houseCenterY;
        // Stagger release timers (random-ish)
        ghostReleaseTimer[0] = 0.5f;  // Blinky
        ghostReleaseTimer[1] = 1.5f;  // Pinky
        ghostReleaseTimer[2] = 2.5f;  // Inky
        ghostReleaseTimer[3] = 3.5f;  // Clyde
        for (int i = 0; i < 4; ++i) ghostState[i] = GhostState::InHouse;

        initializedPositions = true;
    }

    // Lethal collisions when ghosts are not frightened
    auto lethalCollide = [&](int playerIdx, Vec2 ppos, Vec2 gpos, bool ghostFrightened) {
        if (ghostFrightened) return;
        if (deathTimer[playerIdx] > 0.f) return;
        float radius = scaledTileSize * 0.5f * 0.8f;
        float dx     = ppos.x - gpos.x;
        float dy     = ppos.y - gpos.y;
        if (dx * dx + dy * dy <= radius * radius) {
            // kill player
            lives[playerIdx]      = std::max(0, lives[playerIdx] - 1);
            deathTimer[playerIdx] = 3.0f;  // freeze 3s for animation on client
            // respawn at end of timer
            if (lives[playerIdx] == 0) {
                gameOver = true;
            }
        }
    };

    // Check lethal collisions against all ghosts
    Vec2 p0pos = players[0].getPosition();
    Vec2 p1pos = players[1].getPosition();
    lethalCollide(0, p0pos, blinky.getPosition(), blinky.isFrightened());
    lethalCollide(0, p0pos, pinky.getPosition(), pinky.isFrightened());
    lethalCollide(0, p0pos, inky.getPosition(), inky.isFrightened());
    lethalCollide(0, p0pos, clyde.getPosition(), clyde.isFrightened());
    lethalCollide(1, p1pos, blinky.getPosition(), blinky.isFrightened());
    lethalCollide(1, p1pos, pinky.getPosition(), pinky.isFrightened());
    lethalCollide(1, p1pos, inky.getPosition(), inky.isFrightened());
    lethalCollide(1, p1pos, clyde.getPosition(), clyde.isFrightened());

    // Tick death timers and respawn players
    for (int i = 0; i < 2; ++i) {
        if (deathTimer[i] > 0.f) {
            deathTimer[i] -= dt;
            if (deathTimer[i] <= 0.f) {
                players[i].setPosition(spawnX[i], spawnY[i]);
                // clear power for that player
                powerTimer[i] = 0.f;
            }
        }
    }

    if (deathTimer[0] <= 0.f) players[0].update(dt, level, scaledTileSize, scale);
    if (deathTimer[1] <= 0.f) players[1].update(dt, level, scaledTileSize, scale);
    // Update ghosts
    auto p0                  = players[0].getPosition();
    auto p1                  = players[1].getPosition();
    auto updateGhostIfActive = [&](GhostBase& g, int idx) {
        // Handle respawn waiting (after being eaten)
        if (ghostRespawn[idx] > 0.f) {
            g.setFrightened(0.f);
            // keep them positioned at house center while waiting
            g.setPosition(houseCenterX, houseCenterY);
            return;
        }
        // Handle house release state machine
        if (ghostState[idx] == GhostState::InHouse) {
            ghostReleaseTimer[idx] -= dt;
            if (ghostReleaseTimer[idx] <= 0.f) {
                ghostState[idx] = GhostState::Exiting;
            }
        }
        if (ghostState[idx] == GhostState::Exiting) {
            // Move toward computed exit (outside of walls). Keep alignment to lane center to avoid drift.
            Vec2 pos = g.getPosition();
            // Lock horizontal to exit column to ensure perfect centering while exiting vertically
            if (std::abs(pos.x - houseExitX) > 0.01f) {
                pos.x = houseExitX;
                g.setPosition(pos.x, pos.y);
            }
            float dy = houseExitY - pos.y;
            if (std::abs(dy) < scaledTileSize * 0.25f) {
                // Snap to exact tile center at doorway and switch to roaming
                const int   gridW = level.getWidth();
                const int   gridH = level.getHeight();
                const float offX  = (800.f - gridW * scaledTileSize) / 2.f;
                const float offY  = (600.f - gridH * scaledTileSize) / 2.f;
                int         curX  = static_cast<int>(std::floor((pos.x - offX) / scaledTileSize));
                int         curY  = static_cast<int>(std::floor((pos.y - offY) / scaledTileSize));
                float       cx    = offX + curX * scaledTileSize + scaledTileSize * 0.5f;
                float       cy    = offY + curY * scaledTileSize + scaledTileSize * 0.5f;
                g.setPosition(cx, cy);
                ghostState[idx] = GhostState::Roaming;
            } else {
                // Step straight toward exit along Y only
                float speed = 64.f * 6.f * scale;
                float step  = std::copysign(speed * dt, dy);
                // Do not overshoot
                if (std::abs(step) > std::abs(dy)) step = dy;
                g.setPosition(pos.x, pos.y + step);
                return;  // don't run roam logic yet
            }
        }
        // Roaming
        g.update(dt, level, scaledTileSize, scale, p0, players[0].getFacing(), p1);
    };
    updateGhostIfActive(blinky, 0);
    updateGhostIfActive(pinky, 1);
    updateGhostIfActive(inky, 2);
    updateGhostIfActive(clyde, 3);

    // collected pellet deltas for this tick
    consumedThisTick.clear();

    // Pellet and power-pellet collection and timers
    auto handlePlayer = [&](int idx) {
        // Convert position to grid cell
        const int   gridW    = level.getWidth();
        const int   gridH    = level.getHeight();
        const float offX     = (800.f - gridW * scaledTileSize) / 2.f;
        const float offY     = (600.f - gridH * scaledTileSize) / 2.f;
        auto        p        = players[idx].getPosition();
        int         curX     = static_cast<int>(std::floor((p.x - offX) / scaledTileSize));
        int         curY     = static_cast<int>(std::floor((p.y - offY) / scaledTileSize));
        TileType    consumed = level.collectPelletTyped(curX, curY);
        if (consumed == TileType::Pellet) {
            award(idx, ScoreEvent::Pellet);
            consumedThisTick.push_back({curX, curY, consumed});
        } else if (consumed == TileType::PowerPellet) {
            award(idx, ScoreEvent::PowerPellet);
            powerTimer[idx] = 10.0f;  // 10 seconds of power
            // frighten all ghosts for the same duration
            blinky.setFrightened(10.0f);
            pinky.setFrightened(10.0f);
            inky.setFrightened(10.0f);
            clyde.setFrightened(10.0f);
            frightenedEatCount[0] = 0;
            frightenedEatCount[1] = 0;
            consumedThisTick.push_back({curX, curY, consumed});
        } else if (consumed == TileType::Cherry) {
            award(idx, ScoreEvent::Cherry);
            consumedThisTick.push_back({curX, curY, consumed});
        }
        if (powerTimer[idx] > 0.f) {
            powerTimer[idx] -= dt;
            if (powerTimer[idx] < 0.f) powerTimer[idx] = 0.f;
        }
    };
    if (deathTimer[0] <= 0.f) handlePlayer(0);
    if (deathTimer[1] <= 0.f) handlePlayer(1);

    // Handle collisions between players and ghosts
    auto collideCircle = [](Vec2 p, Vec2 g, float radius) {
        float dx = p.x - g.x;
        float dy = p.y - g.y;
        return (dx * dx + dy * dy) <= (radius * radius);
    };
    auto handleGhostEaten = [&](int playerIdx, int ghostIdx) {
        // Points escalate per frightened session for that player
        static const int values[] = {100, 200, 400, 800, 1600};
        int&             count    = frightenedEatCount[playerIdx];
        int              pts      = values[std::min(count, 4)];
        score[playerIdx] += pts;
        count++;
        // Respawn ghost at home spot
        switch (ghostIdx) {
            case 0:
                blinky.setPosition(ghostHomeX, ghostHomeY);
                blinky.setFrightened(0.f);
                ghostRespawn[0] = 5.0f;
                break;
            case 1:
                pinky.setPosition(ghostHomeX, ghostHomeY);
                pinky.setFrightened(0.f);
                ghostRespawn[1] = 5.0f;
                break;
            case 2:
                inky.setPosition(ghostHomeX, ghostHomeY);
                inky.setFrightened(0.f);
                ghostRespawn[2] = 5.0f;
                break;
            case 3:
                clyde.setPosition(ghostHomeX, ghostHomeY);
                clyde.setFrightened(0.f);
                ghostRespawn[3] = 5.0f;
                break;
            default:
                break;
        }
        eatenGhostsThisTick.push_back({ghostHomeX, ghostHomeY, pts});
    };

    bool anyPowered = (powerTimer[0] > 0.f) || (powerTimer[1] > 0.f);
    if (anyPowered) {
        // Use a conservative collision radius ~ half tile
        float radius = scaledTileSize * 0.5f * 0.8f;
        Vec2  p0pos  = players[0].getPosition();
        Vec2  p1pos  = players[1].getPosition();
        Vec2  bpos   = blinky.getPosition();
        Vec2  ppos   = pinky.getPosition();
        Vec2  ipos   = inky.getPosition();
        Vec2  cpos   = clyde.getPosition();
        if (blinky.isFrightened()) {
            if (collideCircle(p0pos, bpos, radius)) handleGhostEaten(0, 0);
            if (collideCircle(p1pos, bpos, radius)) handleGhostEaten(1, 0);
        }
        if (pinky.isFrightened()) {
            if (collideCircle(p0pos, ppos, radius)) handleGhostEaten(0, 1);
            if (collideCircle(p1pos, ppos, radius)) handleGhostEaten(1, 1);
        }
        if (inky.isFrightened()) {
            if (collideCircle(p0pos, ipos, radius)) handleGhostEaten(0, 2);
            if (collideCircle(p1pos, ipos, radius)) handleGhostEaten(1, 2);
        }
        if (clyde.isFrightened()) {
            if (collideCircle(p0pos, cpos, radius)) handleGhostEaten(0, 3);
            if (collideCircle(p1pos, cpos, radius)) handleGhostEaten(1, 3);
        }
    }

    // Tick down ghost respawn timers
    for (int i = 0; i < 4; ++i) {
        if (ghostRespawn[i] > 0.f) {
            ghostRespawn[i] -= dt;
            if (ghostRespawn[i] < 0.f) ghostRespawn[i] = 0.f;
            // keep in house while waiting; when timer ends, reset state to InHouse with a small release delay
            if (ghostRespawn[i] == 0.f) {
                ghostState[i]        = GhostState::InHouse;
                ghostReleaseTimer[i] = 1.0f + 0.5f * i;  // stagger
            }
        }
    }
}

PlayerStateView Simulation::getPlayerState(int playerIndex) const {
    PlayerStateView v{};
    if (playerIndex < 0 || playerIndex > 1) return v;
    auto p          = players[playerIndex].getPosition();
    v.position      = p;
    v.facing        = players[playerIndex].getFacing();
    v.score         = score[playerIndex];
    v.powered       = powerTimer[playerIndex] > 0.f;
    v.powerTimeLeft = powerTimer[playerIndex];
    v.livesLeft     = lives[playerIndex];
    v.deathTimeLeft = deathTimer[playerIndex];
    return v;
}

void Simulation::drainConsumed(std::vector<ConsumedPellet>& out) {
    out = std::move(consumedThisTick);
    consumedThisTick.clear();
}

void Simulation::drainEatenGhosts(std::vector<EatenGhostEvent>& out) {
    out = std::move(eatenGhostsThisTick);
    eatenGhostsThisTick.clear();
}

void Simulation::award(int playerIndex, ScoreEvent eventType) {
    if (playerIndex < 0 || playerIndex > 1) return;
    switch (eventType) {
        case ScoreEvent::Pellet:
            score[playerIndex] += POINTS_PELLET;
            break;
        case ScoreEvent::PowerPellet:
            score[playerIndex] += POINTS_POWER_PELLET;
            break;
        case ScoreEvent::Ghost:
            score[playerIndex] += POINTS_GHOST;
            break;
        case ScoreEvent::Cherry:
            score[playerIndex] += POINTS_CHERRY;
            break;
        default:
            break;
    }
}

Vec2 Simulation::getGhostPosition(int ghostIndex) const {
    switch (ghostIndex) {
        case 0:
            return blinky.getPosition();
        case 1:
            return pinky.getPosition();
        case 2:
            return inky.getPosition();
        case 3:
            return clyde.getPosition();
        default: {
            Vec2 v{};
            return v;
        }
    }
}

Direction Simulation::getGhostFacing(int ghostIndex) const {
    switch (ghostIndex) {
        case 0:
            return blinky.getFacing();
        case 1:
            return pinky.getFacing();
        case 2:
            return inky.getFacing();
        case 3:
            return clyde.getFacing();
        default:
            return Direction::None;
    }
}

bool Simulation::isGhostActive(int ghostIndex) const {
    if (ghostIndex < 0 || ghostIndex > 3) return false;
    return ghostRespawn[ghostIndex] <= 0.f;
}
