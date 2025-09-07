#pragma once

#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"

struct Vec2;

class GhostBase {
   public:
    virtual ~GhostBase() = default;

    void setPosition(float x, float y) {
        positionX = x;
        positionY = y;
    }
    Vec2 getPosition() const;

    void setFrightened(float seconds) {
        frightenedTimer = seconds;
    }
    bool isFrightened() const {
        return frightenedTimer > 0.f;
    }

    // Update with simple greedy targeting behavior implemented by derived class
    void update(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                Direction pac0Facing, Vec2 pac1Pos);

   protected:
    virtual std::pair<int, int> targetTile(const LevelManager& level, float scaledTileSize, Vec2 pac0Pos,
                                           Direction pac0Facing, Vec2 pac1Pos) const = 0;

    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) const;

    float     positionX{0.f}, positionY{0.f};
    Direction direction{Direction::Left};
    float     speed{64.f * 5.5f};
    float     frightenedTimer{0.f};
};

// Four concrete ghosts with simple target logic
class Blinky : public GhostBase {
   protected:
    std::pair<int, int> targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2) const override;
};

class Pinky : public GhostBase {
   protected:
    std::pair<int, int> targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing,
                                   Vec2) const override;
};

class Inky : public GhostBase {
   protected:
    std::pair<int, int> targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction,
                                   Vec2 pac1Pos) const override;
};

class Clyde : public GhostBase {
   protected:
    std::pair<int, int> targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2) const override;
};
