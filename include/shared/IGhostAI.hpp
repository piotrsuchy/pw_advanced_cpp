#pragma once

#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

/**
 * @brief Strategy interface for per-ghost targeting behavior.
 *
 * Each concrete AI implementation defines both chase targeting and the fixed
 * scatter corner used by `Ghost` during mode transitions.
 */
class IGhostAI {
   public:
    /**
     * @brief Virtual destructor for polymorphic AI strategies.
     */
    virtual ~IGhostAI() = default;

    /**
     * @brief Computes the chase target tile for a ghost.
     *
     * @param level Active level used for bounds and corners.
     * @param tile Tile size in logic coordinates.
     * @param pac0Pos Player 0 position.
     * @param pac0Facing Player 0 facing direction.
     * @param pac1Pos Player 1 position.
     * @param currentGhostPos Position of the ghost being updated.
     * @return Target tile coordinates as `(x, y)`.
     */
    virtual std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction pac0Facing,
                                              Vec2 pac1Pos, Vec2 currentGhostPos) const = 0;

    /**
     * @brief Returns the fixed scatter corner for this ghost.
     *
     * @param level Active level used to derive edge coordinates.
     * @return Scatter target tile coordinates as `(x, y)`.
     */
    virtual std::pair<int, int> getScatterTile(const LevelManager& level) const = 0;
};
