#pragma once

#include "shared/GameTypes.hpp"

struct Vec2;  // Forward declaration from PacmanLogic.hpp or GameTypes.hpp

// Pure virtual interface for all game entities
class IEntity {
   public:
    virtual ~IEntity() = default;

    // Logic update step
    virtual void update(float dt) = 0;

    // Get the entity's position in grid/logic coordinates
    virtual Vec2 getPosition() const = 0;

    // Set the entity's position
    virtual void setPosition(float x, float y) = 0;

    // Get the direction the entity is currently facing
    virtual Direction getFacing() const = 0;
};
