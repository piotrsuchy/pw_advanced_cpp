#pragma once

#include <array>
#include <vector>

#include "shared/GameTypes.hpp"
#include "shared/GhostLogic.hpp"
#include "shared/Player.hpp"

// Points awarded per ghost eat in a combo (index capped at 4)
static constexpr int GHOST_EAT_VALUES[] = {100, 200, 400, 800, 1600};

// A record of a ghost being eaten (for score popups, etc.)
struct GhostEatenEvent {
    float x;
    float y;
    int   points;
    int   ghostIndex;
};

// The InteractionResolver is responsible for detecting and resolving
// all interactions between game entities each simulation tick.
// This keeps collision/interaction logic entirely separate from the
// Pacman movement logic and ghost AI.
class InteractionResolver {
   public:
    // Check and apply Pac-Man vs Ghost lethal collisions.
    // Returns true if any player was killed.
    static bool resolveLethal(std::array<Player, 2>& players, std::array<std::unique_ptr<Ghost>, 4>& ghosts,
                              float collisionRadius);

    // Check and apply Pac-Man vs Ghost frightened collisions (eating ghosts).
    // Populates the out-param with events for each ghost that was eaten.
    static void resolveFrightened(std::array<Player, 2>& players, std::array<std::unique_ptr<Ghost>, 4>& ghosts,
                                  float collisionRadius, float ghostHomeX, float ghostHomeY,
                                  std::vector<GhostEatenEvent>& outEvents, std::array<float, 4>& ghostRespawnTimers);

   private:
    static bool circleCollide(Vec2 a, Vec2 b, float radius);
};
