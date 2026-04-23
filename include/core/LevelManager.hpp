#pragma once
#include <cstdint>
#include <vector>

class ICollectible;

/**
 * @brief Tile identifiers used by the level grid.
 */
enum class TileType : int {
    Empty       = 0,  ///< Walkable empty tile.
    Wall        = 1,  ///< Solid wall tile that blocks movement.
    Pellet      = 2,  ///< Standard pellet tile worth the base pellet score.
    PowerPellet = 3,  ///< Power pellet tile that activates frightened mode.
    BonusFruit1 = 4,  ///< Bonus fruit tile worth 100 points.
    BonusFruit2 = 5,  ///< Bonus fruit tile worth 200 points.
    BonusFruit3 = 6,  ///< Bonus fruit tile worth 400 points.
    BonusFruit4 = 7,  ///< Bonus fruit tile worth 800 points.
};

/**
 * @brief Converts a raw integer tile value into a `TileType`.
 *
 * @param v Raw tile value.
 * @return The corresponding `TileType`.
 */
static TileType fromInt(int v) {
    return static_cast<TileType>(v);
}

/**
 * @brief Owns the tile grid and collectible state for the active level.
 *
 * This class loads the maze layout, exposes wrapped tile access, and handles
 * pellet collection and tunnel-related derived data.
 */
class LevelManager {
   public:
    /**
     * @brief Loads one of the built-in level layouts.
     *
     * @param levelNumber One-based level index to initialize.
     */
    void loadLevel(int levelNumber);

    /**
     * @brief Replaces the current grid with an integer-encoded layout.
     *
     * @param intGrid Two-dimensional raw tile matrix.
     */
    void loadFromInts(const std::vector<std::vector<int>>& intGrid);

    /**
     * @brief Returns the grid width in tiles.
     *
     * @return Number of columns in the loaded level.
     */
    int getWidth() const {
        return static_cast<int>(grid.empty() ? 0 : grid[0].size());
    }

    /**
     * @brief Returns the grid height in tiles.
     *
     * @return Number of rows in the loaded level.
     */
    int getHeight() const {
        return static_cast<int>(grid.size());
    }

    /**
     * @brief Reads a tile from the grid with horizontal wraparound.
     *
     * Vertical out-of-bounds reads are treated as walls to simplify movement
     * and collision logic.
     *
     * @param x Tile x-coordinate.
     * @param y Tile y-coordinate.
     * @return Tile value at the wrapped position.
     */
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

    /**
     * @brief Writes a tile if the requested position is inside the grid.
     *
     * @param x Tile x-coordinate.
     * @param y Tile y-coordinate.
     * @param type New tile value.
     */
    void setTile(int x, int y, TileType type) {
        if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
            return;
        }
        grid[y][x] = type;
    }

    /**
     * @brief Collects a pellet at the given tile, if present.
     *
     * @param x Tile x-coordinate.
     * @param y Tile y-coordinate.
     * @return `true` if a collectible pellet tile was cleared.
     */
    bool collectPellet(int x, int y);

    /**
     * @brief Collects any supported collectible at the given tile.
     *
     * The tile is cleared when a collectible exists at the requested position.
     *
     * @param x Tile x-coordinate.
     * @param y Tile y-coordinate.
     * @return Pointer to the collected item definition, or `nullptr` if the
     * tile did not contain a collectible.
     */
    const ICollectible* collectAt(int x, int y);

    /**
     * @brief Counts the remaining pellets required to clear the level.
     *
     * Bonus fruit tiles are excluded from this total.
     *
     * @return Number of remaining pellets and power pellets.
     */
    int getRemainingPellets() const;

    /**
     * @brief Checks whether a row is the identified side-tunnel row.
     *
     * Ghost movement logic uses this to apply the tunnel speed reduction.
     *
     * @param tileY Tile row to test.
     * @return `true` when the row matches the tunnel row.
     */
    bool isHorizontalGhostTunnelRow(int tileY) const {
        return ghostTunnelRowY_ >= 0 && tileY == ghostTunnelRowY_;
    }

   private:
    void recomputeDerivedFields();

    std::vector<std::vector<TileType>> grid;
    int                                ghostTunnelRowY_{-1};
};