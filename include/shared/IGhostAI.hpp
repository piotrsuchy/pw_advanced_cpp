#pragma once

#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

// Forward declaration
class GhostBase;

// Interface for Ghost targeting strategies
class IGhostAI {
   public:
    virtual ~IGhostAI() = default;

    // Returns the target tile (x, y) that the ghost should aim for.
    virtual std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction pac0Facing,
                                              Vec2 pac1Pos, Vec2 currentGhostPos) const = 0;
};
