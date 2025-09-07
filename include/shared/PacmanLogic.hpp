#pragma once

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

struct Vec2 {
    float x{0.f};
    float y{0.f};
};

class PacmanLogic {
   public:
    void setPosition(float x, float y) {
        position = {x, y};
    }
    Vec2 getPosition() const {
        return position;
    }
    Direction getDirection() const {
        return direction;
    }
    Direction getFacing() const {
        return facingDirection;
    }
    void setDesired(Direction d) {
        if (d != Direction::None) desiredDirection = d;
    }
    void update(float dt, LevelManager& level, float scaledTileSize, float scale);

   private:
    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY);

    Vec2      position{0.f, 0.f};
    Direction direction{Direction::None};
    Direction desiredDirection{Direction::None};
    Direction facingDirection{Direction::Right};
    float     speed{384.f};
};
