#pragma once

#include "shared/GameTypes.hpp"

struct Vec2;  // Forward declaration from PacmanLogic.hpp or GameTypes.hpp

/**
 * @brief Common logic-side interface implemented by movable game entities.
 */
class IEntity {
   public:
    /**
     * @brief Virtual destructor for polymorphic entity usage.
     */
    virtual ~IEntity() = default;

    /**
     * @brief Advances the entity by one logic step.
     *
     * @param dt Elapsed time in seconds.
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Returns the entity position in logic-space coordinates.
     *
     * @return Current position.
     */
    virtual Vec2 getPosition() const = 0;

    /**
     * @brief Sets the entity position.
     *
     * @param x X coordinate in logic space.
     * @param y Y coordinate in logic space.
     */
    virtual void setPosition(float x, float y) = 0;

    /**
     * @brief Returns the direction the entity is currently facing.
     *
     * @return Current facing direction.
     */
    virtual Direction getFacing() const = 0;
};
