#pragma once

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

/**
 * @brief Owns Pac-Man movement state and tile-based steering logic.
 */
class PacmanLogic {
   public:
    /**
     * @brief Sets the current Pac-Man position.
     *
     * @param x X coordinate in logic space.
     * @param y Y coordinate in logic space.
     */
    void setPosition(float x, float y) {
        position = {x, y};
    }

    /**
     * @brief Returns the current position.
     *
     * @return Current logic-space position.
     */
    Vec2 getPosition() const {
        return position;
    }

    /**
     * @brief Returns the currently applied movement direction.
     *
     * @return Current movement direction.
     */
    Direction getDirection() const {
        return direction;
    }

    /**
     * @brief Returns the direction Pac-Man is facing for rendering/targeting.
     *
     * @return Current facing direction.
     */
    Direction getFacing() const {
        return facingDirection;
    }

    /**
     * @brief Queues a new desired movement direction.
     *
     * `Direction::None` is ignored so the previous intent stays active.
     *
     * @param d Desired direction.
     */
    void setDesired(Direction d) {
        if (d != Direction::None) desiredDirection = d;
    }

    /**
     * @brief Clears both the current and desired movement directions.
     */
    void resetDirection() {
        direction        = Direction::None;
        desiredDirection = Direction::None;
    }

    /**
     * @brief Advances Pac-Man movement for one fixed simulation step.
     *
     * @param dt Elapsed time in seconds.
     * @param level Mutable level used for movement and tunnel checks.
     * @param scaledTileSize Tile size after scaling.
     * @param scale World scale factor.
     */
    void update(float dt, LevelManager& level, float scaledTileSize, float scale);

   private:
    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY);
    bool canCornerTurn(Direction newDir, float tile, float offX, float offY, const LevelManager& lvl) const;
    bool isPerpendicular(Direction a, Direction b) const;

    static constexpr float CORNER_TOLERANCE = 8.f;  // pixels from tile center to allow cornering

    Vec2      position{0.f, 0.f};
    Direction direction{Direction::None};
    Direction desiredDirection{Direction::None};
    Direction facingDirection{Direction::Right};
    float     speed{384.f};
};
