#pragma once

#include <memory>
#include <utility>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"
#include "shared/IEntity.hpp"
#include "shared/IGhostAI.hpp"

/**
 * @brief Logic-side ghost entity driven by an injected AI strategy.
 *
 * The class tracks position, facing, frightened state, and per-tick movement
 * while delegating target selection to an `IGhostAI` implementation.
 */
class Ghost : public IEntity {
   public:
    /**
     * @brief Creates a ghost that uses the supplied AI strategy.
     *
     * @param aiStrategy Strategy object that owns the chase/scatter logic.
     */
    Ghost(std::unique_ptr<IGhostAI> aiStrategy);

    /**
     * @brief Virtual destructor required by the `IEntity` interface.
     */
    ~Ghost() override = default;

    /**
     * @brief Advances internal timers for a lightweight logic update.
     *
     * @param dt Elapsed time in seconds.
     */
    void update(float dt) override;

    /**
     * @brief Returns the current ghost position.
     *
     * @return Current position in logic coordinates.
     */
    Vec2 getPosition() const override;

    /**
     * @brief Sets the current ghost position.
     *
     * @param x X coordinate in logic space.
     * @param y Y coordinate in logic space.
     */
    void setPosition(float x, float y) override;

    /**
     * @brief Returns the ghost's current facing direction.
     *
     * @return Current facing direction.
     */
    Direction getFacing() const override;

    /**
     * @brief Activates frightened mode for the given duration.
     *
     * @param seconds Frightened duration in seconds.
     */
    void setFrightened(float seconds);

    /**
     * @brief Checks whether frightened mode is currently active.
     *
     * @return `true` if the frightened timer is still running.
     */
    bool isFrightened() const;

    /**
     * @brief Immediately reverses the current movement direction.
     */
    void reverseDirection();

    /**
     * @brief Runs the full AI-driven movement update for one tick.
     *
     * The requested `mode` is ignored while frightened mode is active.
     *
     * @param dt Elapsed time in seconds.
     * @param level Active level grid.
     * @param scaledTileSize Tile size after scaling.
     * @param scale World scale factor.
     * @param pac0Pos Player 0 position.
     * @param pac0Facing Player 0 facing direction.
     * @param pac1Pos Player 1 position.
     * @param mode Requested scatter/chase mode.
     * @param cruiseElroyChaseSpeedMul Optional Blinky chase speed multiplier.
     */
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

/**
 * @brief AI strategy for Blinky, who directly targets player 0.
 */

class BlinkyAI : public IGhostAI {
   public:
    /**
     * @brief Returns Blinky's chase target.
     *
     * @param level Active level used to determine bounds.
     * @param tile Tile size in logic coordinates.
     * @param pac0Pos Player 0 position.
     * @param pac0Facing Player 0 facing direction.
     * @param pac1Pos Player 1 position (unused by Blinky).
     * @param currentGhostPos Current Blinky position.
     * @return Target tile coordinates as `(x, y)`.
     */
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2) const override;
    /**
     * @brief Returns Blinky's scatter corner.
     *
     * @param level Active level used to derive the corner tile.
     * @return Top-right scatter tile.
     */
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

/**
 * @brief AI strategy for Pinky, who aims ahead of player 0.
 */
class PinkyAI : public IGhostAI {
   public:
    /**
     * @brief Returns Pinky's chase target.
     *
     * @param level Active level used to determine bounds.
     * @param tile Tile size in logic coordinates.
     * @param pac0Pos Player 0 position.
     * @param facing Player 0 facing direction.
     * @param pac1Pos Player 1 position (unused by Pinky).
     * @param currentGhostPos Current Pinky position.
     * @return Target tile coordinates as `(x, y)`.
     */
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing, Vec2,
                                      Vec2) const override;
    /**
     * @brief Returns Pinky's scatter corner.
     *
     * @param level Active level used to derive the corner tile.
     * @return Top-left scatter tile.
     */
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

/**
 * @brief AI strategy for Inky, whose target depends on both players.
 */
class InkyAI : public IGhostAI {
   public:
    /**
     * @brief Returns Inky's chase target.
     *
     * @param level Active level used to determine bounds.
     * @param tile Tile size in logic coordinates.
     * @param pac0Pos Player 0 position.
     * @param pac0Facing Player 0 facing direction.
     * @param pac1Pos Player 1 position.
     * @param currentGhostPos Current Inky position.
     * @return Target tile coordinates as `(x, y)`.
     */
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2 pac1Pos,
                                      Vec2) const override;
    /**
     * @brief Returns Inky's scatter corner.
     *
     * @param level Active level used to derive the corner tile.
     * @return Bottom-right scatter tile.
     */
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};

/**
 * @brief AI strategy for Clyde, who alternates between chase and retreat.
 */
class ClydeAI : public IGhostAI {
   public:
    /**
     * @brief Returns Clyde's chase target.
     *
     * @param level Active level used to determine bounds.
     * @param tile Tile size in logic coordinates.
     * @param pac0Pos Player 0 position.
     * @param pac0Facing Player 0 facing direction.
     * @param pac1Pos Player 1 position (unused by Clyde).
     * @param currentPos Current Clyde position.
     * @return Target tile coordinates as `(x, y)`.
     */
    std::pair<int, int> getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                      Vec2 currentPos) const override;
    /**
     * @brief Returns Clyde's scatter corner.
     *
     * @param level Active level used to derive the corner tile.
     * @return Bottom-left scatter tile.
     */
    std::pair<int, int> getScatterTile(const LevelManager& level) const override;
};
