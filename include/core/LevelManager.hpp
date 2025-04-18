#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType : int {
    Empty       = 0,
    Wall        = 1,
    Pellet      = 2,
    PowerPellet = 3
};

class LevelManager {
public:
    void loadLevel(int levelNumber);

    int getWidth() const { return static_cast<int>(grid.empty()? 0 : grid[0].size()); }
    int getHeight() const { return static_cast<int>(grid.size()); }

    TileType getTile(int x, int y) const { return grid[y][x]; }
    void setTile(int x, int y, TileType type) { grid[y][x] = type; }

    bool collectPellet(int x, int y);

private:
    std::vector<std::vector<TileType>> grid;
};