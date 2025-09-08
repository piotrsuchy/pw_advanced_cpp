#pragma once
#include <cstdint>
#include <vector>

enum class TileType : int { Empty = 0, Wall = 1, Pellet = 2, PowerPellet = 3, Cherry = 4 };

static TileType fromInt(int v) {
    return static_cast<TileType>(v);
}

class LevelManager {
   public:
    void loadLevel(int levelNumber);
    void loadFromInts(const std::vector<std::vector<int>>& intGrid);

    int getWidth() const {
        return static_cast<int>(grid.empty() ? 0 : grid[0].size());
    }
    int getHeight() const {
        return static_cast<int>(grid.size());
    }

    TileType getTile(int x, int y) const {
        if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
            return TileType::Wall;  // treat out-of-bounds as walls
        }
        return grid[y][x];
    }
    void setTile(int x, int y, TileType type) {
        if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
            return;
        }
        grid[y][x] = type;
    }

    bool collectPellet(int x, int y);
    // Returns the type of pellet consumed at (x,y), or TileType::Empty if none
    TileType collectPelletTyped(int x, int y);

   private:
    std::vector<std::vector<TileType>> grid;
};