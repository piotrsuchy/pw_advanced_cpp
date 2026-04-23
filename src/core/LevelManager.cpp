#include "core/LevelManager.hpp"

#include "core/ClassicMaze.hpp"
#include "core/ICollectible.hpp"

void LevelManager::loadLevel(int lvl) {
    {
        (void)lvl;
        loadFromInts(CLASSIC_MAZE);
    }
}

// LevelManager.cpp
void LevelManager::loadFromInts(const std::vector<std::vector<int>>& intGrid) {
    grid.resize(intGrid.size());
    for (std::size_t y = 0; y < intGrid.size(); ++y) {
        grid[y].resize(intGrid[y].size());
        for (std::size_t x = 0; x < intGrid[y].size(); ++x) {
            grid[y][x] = fromInt(intGrid[y][x]);
        }
    }
    recomputeDerivedFields();
}

void LevelManager::recomputeDerivedFields() {
    ghostTunnelRowY_ = -1;
    const int w      = getWidth();
    const int h      = getHeight();
    if (w == 0 || h == 0) return;
    // Row with open left/right map edges and the longest horizontal run (the wrap tunnel)
    int bestY = 0;
    int best  = 0;
    for (int y = 0; y < h; ++y) {
        if (getTile(0, y) == TileType::Wall || getTile(w - 1, y) == TileType::Wall) continue;
        int len = 0;
        for (int x = 0; x < w; ++x) {
            if (getTile(x, y) != TileType::Wall) ++len;
        }
        if (len > best) {
            best  = len;
            bestY = y;
        }
    }
    if (best > 0) ghostTunnelRowY_ = bestY;
}

bool LevelManager::collectPellet(int x, int y) {
    return collectAt(x, y) != nullptr;
}

const ICollectible* LevelManager::collectAt(int x, int y) {
    if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
        return nullptr;
    }
    const ICollectible* c = collectibleForTileType(static_cast<int>(grid[y][x]));
    if (!c) {
        return nullptr;
    }
    grid[y][x] = TileType::Empty;
    return c;
}

int LevelManager::getRemainingPellets() const {
    int count = 0;
    for (const auto& row : grid) {
        for (TileType tile : row) {
            if (const ICollectible* c = collectibleForTileType(static_cast<int>(tile));
                c && c->countsTowardLevelPelletTotal()) {
                ++count;
            }
        }
    }
    return count;
}
