#pragma once

#include <memory>
#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"
#include "shared/IEntity.hpp"
#include "shared/IGhostAI.hpp"

class Ghost : public IEntity {
   public:
    Ghost(std::unique_ptr<IGhostAI> aiStrategy);
    ~Ghost() override = default;

    // IEntity implementation
    void      update(float dt) override;
    Vec2      getPosition() const override;
    void      setPosition(float x, float y) override;
    Direction getFacing() const override;

    // Ghost specific
    void setFrightened(float seconds);
    bool isFrightened() const;

    // Instead of simple update(dt), Simulation needs to pass context for the AI.
    // We will provide a specific update method for the ghost logic.
    void updateLogic(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                     Direction pac0Facing, Vec2 pac1Pos);

   private:
    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) const;

    std::unique_ptr<IGhostAI> ai;
    Vec2                      position{0.f, 0.f};
    Direction                 direction{Direction::Left};
    float                     speed{64.f * 5.5f};
    float                     frightenedTimer{0.f};
};

// Implementations of the AI strategies
class BlinkyAI : public IGhostAI {
   public:
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2) const override;
};

class PinkyAI : public IGhostAI {
   public:
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing, Vec2,
                                      Vec2) const override;
};

class InkyAI : public IGhostAI {
   public:
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2 pac1Pos,
                                      Vec2) const override;
};

class ClydeAI : public IGhostAI {
   public:
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2 currentPos) const override;
};
