#pragma once

#include <array>
#include <vector>

#include "shared/GameTypes.hpp"
#include "shared/GhostLogic.hpp"
#include "shared/Player.hpp"

/**
 * @brief Ghost combo score values used during frightened mode.
 */
static constexpr int GHOST_EAT_VALUES[] = {100, 200, 400, 800, 1600};

/**
 * @brief Event emitted when a frightened ghost gets eaten.
 */
struct GhostEatenEvent {
    float x;           ///< Popup world X coordinate.
    float y;           ///< Popup world Y coordinate.
    int   points;      ///< Score awarded for eating the ghost.
    int   ghostIndex;  ///< Ghost slot index that was eaten.
};

/**
 * @brief Resolves player-versus-ghost interactions for a simulation tick.
 *
 * This keeps collision consequences separate from movement and targeting code.
 */
class InteractionResolver {
   public:
    /**
     * @brief Applies lethal collisions between players and ghosts.
     *
     * @param players Active player states.
     * @param ghosts Active ghost instances.
     * @param collisionRadius Collision radius used for overlap checks.
     * @return `true` if at least one player lost a life.
     */
    static bool resolveLethal(std::array<Player, 2>& players, std::array<std::unique_ptr<Ghost>, 4>& ghosts,
                              float collisionRadius);

    /**
     * @brief Applies frightened-mode collisions where players can eat ghosts.
     *
     * @param players Active player states.
     * @param ghosts Active ghost instances.
     * @param collisionRadius Collision radius used for overlap checks.
     * @param ghostHomeX Ghost house X position used for respawn.
     * @param ghostHomeY Ghost house Y position used for respawn.
     * @param outEvents Output collection for score popup events.
     * @param ghostRespawnTimers Output respawn timers indexed by ghost slot.
     */
    static void resolveFrightened(std::array<Player, 2>& players, std::array<std::unique_ptr<Ghost>, 4>& ghosts,
                                  float collisionRadius, float ghostHomeX, float ghostHomeY,
                                  std::vector<GhostEatenEvent>& outEvents, std::array<float, 4>& ghostRespawnTimers);

   private:
    static bool circleCollide(Vec2 a, Vec2 b, float radius);
};
