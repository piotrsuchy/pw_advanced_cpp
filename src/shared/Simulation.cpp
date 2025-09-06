#include "shared/Simulation.hpp"

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

        initializedPositions = true;
    }

    players[0].update(dt, level, scaledTileSize, scale);
    players[1].update(dt, level, scaledTileSize, scale);
}

PlayerStateView Simulation::getPlayerState(int playerIndex) const {
    PlayerStateView v{};
    if (playerIndex < 0 || playerIndex > 1) return v;
    auto p     = players[playerIndex].getPosition();
    v.position = p;
    v.facing   = players[playerIndex].getFacing();
    return v;
}
