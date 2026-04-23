#pragma once
#include <cstdint>
#include <vector>

class ICollectible;

enum class TileType : int {
    Empty       = 0,
    Wall        = 1,
    Pellet      = 2,
    PowerPellet = 3,
    BonusFruit1 = 4,  // 100 pts
    BonusFruit2 = 5,  // 200 pts
    BonusFruit3 = 6,  // 400 pts
    BonusFruit4 = 7,  // 800 pts
};

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
        if (y < 0 || y >= getHeight()) {
            return TileType::Wall;  // vertical out-of-bounds is a wall
        }
        const int w = getWidth();
        if (w == 0) return TileType::Wall;
        // Wrap x horizontally — enables tunnel traversal
        x = ((x % w) + w) % w;
        return grid[y][x];
    }
    void setTile(int x, int y, TileType type) {
        if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
            return;
        }
        grid[y][x] = type;
    }

    bool collectPellet(int x, int y);
    // Picks up whatever collectible is at (x,y), clears the cell; nullptr if nothing there.
    const ICollectible* collectAt(int x, int y);

    // Returns count of remaining pellets (Pellet + PowerPellet, excludes bonus fruit)
    int getRemainingPellets() const;

    /// True when `tileY` is the row where side tunnels connect (used for ghost slowdown).
    bool isHorizontalGhostTunnelRow(int tileY) const {
        return ghostTunnelRowY_ >= 0 && tileY == ghostTunnelRowY_;
    }

   private:
    void recomputeDerivedFields();

    std::vector<std::vector<TileType>> grid;
    int                                ghostTunnelRowY_{-1};
};