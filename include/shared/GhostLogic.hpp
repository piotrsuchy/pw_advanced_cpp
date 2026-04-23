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

    // Immediately reverse the current direction (called on scatter↔chase switch).
    void reverseDirection();

    // Full AI-driven update.  mode is ignored while frightened.
    /// `cruiseElroyChaseSpeedMul` is Cruise Elroy (classic): Blinky-only chase speed-up when few pellets
    /// remain; always pass 1 for non-Blinky (Simulation sets e.g. 1.08 / 1.12 for Blinky in chase).
    void updateLogic(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                     Direction pac0Facing, Vec2 pac1Pos, GhostMode mode, float cruiseElroyChaseSpeedMul = 1.f);

   private:
    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) const;

    std::unique_ptr<IGhostAI> ai;
    Vec2                      position{0.f, 0.f};
    Direction                 direction{Direction::Left};
    float                     speed{64.f * 5.5f};
    float                     frightenedTimer{0.f};
};

// ── AI strategy implementations ─────────────────────────────────────────────

class BlinkyAI : public IGhostAI {
   public:
    // Chase: directly target player 0.
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2) const override;
    // Scatter corner: top-right.
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

class PinkyAI : public IGhostAI {
   public:
    // Chase: 4 tiles ahead of player 0.
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing, Vec2,
                                      Vec2) const override;
    // Scatter corner: top-left.
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

class InkyAI : public IGhostAI {
   public:
    // Chase: roughly between both players.
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2 pac1Pos,
                                      Vec2) const override;
    // Scatter corner: bottom-right.
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

class ClydeAI : public IGhostAI {
   public:
    // Chase: target player 0 when far, retreat to corner when close.
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2 currentPos) const override;
    // Scatter corner: bottom-left.
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};
