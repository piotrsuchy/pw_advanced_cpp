#include "core/LevelManager.hpp"

void LevelManager::loadLevel(int) {
    grid = {
        {TileType::Wall, TileType::Wall, TileType::Wall },
        {TileType::Wall, TileType::Wall, TileType::Wall },
        {TileType::Wall, TileType::Wall, TileType::Wall }
    };
}

bool LevelManager::collectPellet(int x, int y) {
    if (grid[y][x] == TileType::Pellet || grid[y][x] == TileType::PowerPellet) {
        grid[y][x] = TileType::Empty;
        return true;
    }
    return false;
}