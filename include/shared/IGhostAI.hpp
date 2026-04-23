#pragma once

#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

// Interface for Ghost targeting strategies.
// Each concrete AI encapsulates both chase logic (getTargetTile) and its
// scatter corner (getScatterTile), so switching modes only requires
// querying the right method — no branching outside the AI.
class IGhostAI {
   public:
    virtual ~IGhostAI() = default;

    // Returns the chase target tile (x, y) for this ghost.
    virtual std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction pac0Facing,
                                              Vec2 pac1Pos, Vec2 currentGhostPos) const = 0;

    // Returns this ghost's fixed scatter corner tile (x, y).
    virtual std::pair<int, int> getScatterTile(const LevelManager& level) const = 0;
};
