#include "core/LevelManager.hpp"

#include "core/ClassicMaze.hpp"

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
    if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
        return false;
    }
    if (grid[y][x] == TileType::Pellet || grid[y][x] == TileType::PowerPellet) {
        grid[y][x] = TileType::Empty;
        return true;
    }
    return false;
}
