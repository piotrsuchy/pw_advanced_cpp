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
