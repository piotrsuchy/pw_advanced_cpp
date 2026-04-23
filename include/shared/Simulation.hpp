#pragma once

#include <array>
#include <memory>
#include <random>
#include <vector>

#include "core/LevelManager.hpp"

class ICollectible;
#include "shared/EventQueue.hpp"
#include "shared/GameTypes.hpp"
#include "shared/GhostLogic.hpp"
#include "shared/InteractionResolver.hpp"
#include "shared/Player.hpp"

/**
 * @brief Snapshot-friendly view of one player's state.
 */
struct PlayerStateView {
    Vec2      position;            ///< Current player position in logic space.
    Direction facing;              ///< Current player facing direction.
    int       score{0};            ///< Current score total.
    bool      powered{false};      ///< Whether a power pellet is active.
    float     powerTimeLeft{0.f};  ///< Remaining power-pellet duration in seconds.
    int       livesLeft{3};        ///< Remaining lives for the player.
    float     deathTimeLeft{0.f};  ///< Remaining death animation or respawn delay in seconds.
};

/**
 * @brief Categories of score events emitted by the simulation.
 */
enum class ScoreEvent {
    Pellet,       ///< Standard pellet score event.
    PowerPellet,  ///< Power pellet score event.
    Ghost,        ///< Frightened ghost score popup event.
    BonusFruit,   ///< Bonus fruit score popup event.
};

/**
 * @brief Authoritative gameplay simulation for the active Pac-Man match.
 *
 * `Simulation` owns players, ghosts, the level state, collectible handling,
 * frightened/scatter/chase timing, and the event queues consumed by the server.
 */
class Simulation {
   public:
    /**
     * @brief Constructs an empty simulation instance.
     */
    Simulation();

    /**
     * @brief Loads and initializes a level without resetting the whole match.
     *
     * @param levelNumber One-based level index to initialize.
     */
    void initLevel(int levelNumber);

    /**
     * @brief Resets the full match state and loads the requested level.
     *
     * @param levelNumber One-based level index to initialize after the reset.
     */
    void resetForNewMatch(int levelNumber);

    /**
     * @brief Queues a desired movement direction for one player.
     *
     * @param playerIndex Player slot index (`0` or `1`).
     * @param d Desired direction.
     */
    void setDesired(int playerIndex, Direction d);

    /**
     * @brief Advances the full simulation by one fixed step.
     *
     * @param dt Elapsed time in seconds.
     * @param scaledTileSize Tile size after scaling.
     * @param scale World scale factor.
     */
    void step(float dt, float scaledTileSize, float scale);

    /**
     * @brief Returns a compact read-only view of one player.
     *
     * @param playerIndex Player slot index (`0` or `1`).
     * @return Snapshot-friendly player state view.
     */
    PlayerStateView getPlayerState(int playerIndex) const;

    /**
     * @brief Returns the current level state.
     *
     * @return Read-only reference to the level manager.
     */
    const LevelManager& getLevel() const {
        return level;
    }

    /**
     * @brief Returns the position of a ghost by slot index.
     *
     * @param ghostIndex Ghost slot index (`0` to `3`).
     * @return Current ghost position.
     */
    Vec2 getGhostPosition(int ghostIndex) const;

    /**
     * @brief Returns the facing direction of a ghost by slot index.
     *
     * @param ghostIndex Ghost slot index (`0` to `3`).
     * @return Current ghost facing direction.
     */
    Direction getGhostFacing(int ghostIndex) const;

    /**
     * @brief Checks whether a ghost is currently active in play.
     *
     * @param ghostIndex Ghost slot index (`0` to `3`).
     * @return `true` if the ghost is active outside of respawn.
     */
    bool isGhostActive(int ghostIndex) const;

    /**
     * @brief Checks whether a ghost is currently frightened.
     *
     * @param ghostIndex Ghost slot index (`0` to `3`).
     * @return `true` if frightened mode is active for that ghost.
     */
    bool isGhostFrightened(int ghostIndex) const;

    /**
     * @brief Event describing a collectible that was consumed this tick.
     */
    struct ConsumedPellet {
        int                 x{0};           ///< Tile X coordinate of the consumed collectible.
        int                 y{0};           ///< Tile Y coordinate of the consumed collectible.
        const ICollectible* item{nullptr};  ///< Static collectible definition that was consumed.
    };

    /**
     * @brief Moves consumed-collectible events into an output vector.
     *
     * Drains the internal `EventQueue<ConsumedPellet>` for the current tick.
     *
     * @param out Destination vector receiving the drained events.
     */
    void drainConsumed(std::vector<ConsumedPellet>& out);

    /**
     * @brief Event describing a ghost score popup emitted this tick.
     */
    struct EatenGhostEvent {
        float x;       ///< Popup world X coordinate.
        float y;       ///< Popup world Y coordinate.
        int   points;  ///< Score awarded for the frightened ghost.
    };

    /**
     * @brief Moves ghost-eaten events into an output vector.
     *
     * Drains the internal `EventQueue<EatenGhostEvent>` for the current tick.
     *
     * @param out Destination vector receiving the drained events.
     */
    void drainEatenGhosts(std::vector<EatenGhostEvent>& out);

    /**
     * @brief Event describing a tile value that changed on the grid.
     */
    struct GridTileUpdate {
        std::uint16_t x{0};     ///< Tile X coordinate that changed.
        std::uint16_t y{0};     ///< Tile Y coordinate that changed.
        std::uint8_t  tile{0};  ///< New raw `TileType` value stored at the tile.
    };

    /**
     * @brief Moves pending grid tile updates into an output vector.
     *
     * Drains the internal `EventQueue<GridTileUpdate>` for the current tick.
     *
     * @param out Destination vector receiving the drained tile changes.
     */
    void drainGridTileUpdates(std::vector<GridTileUpdate>& out);

    /**
     * @brief Moves fruit score popup events into an output vector.
     *
     * Drains the internal `EventQueue<EatenGhostEvent>` used for fruit score popups.
     *
     * @param out Destination vector receiving the drained fruit popup events.
     */
    void drainFruitPopups(std::vector<EatenGhostEvent>& out);

    /**
     * @brief Checks whether the match is lost because a player ran out of lives.
     *
     * @return `true` if the match cannot continue.
     */
    bool isMatchLost() const;

    /**
     * @brief Checks whether all clearable pellets on the level are gone.
     *
     * @return `true` if the current level has been cleared.
     */
    bool isLevelCleared() const;

   private:
    enum class GhostState { InHouse, Exiting, Roaming };

    LevelManager level;

    // Players are now proper objects
    std::array<Player, 2> players = {Player(0), Player(1)};

    // Ghosts as unique_ptr to avoid copy-constructor issues with unique_ptr<IGhostAI>
    std::array<std::unique_ptr<Ghost>, 4> ghosts;

    bool initializedPositions{false};

    // Ghost house positions
    float ghostHomeX{0.f};
    float ghostHomeY{0.f};
    float houseCenterX{0.f};
    float houseCenterY{0.f};
    float houseExitX{0.f};
    float houseExitY{0.f};

    GhostState ghostState[4] = {GhostState::InHouse, GhostState::InHouse, GhostState::InHouse, GhostState::InHouse};
    float      ghostReleaseTimer[4]   = {0.f, 0.f, 0.f, 0.f};
    std::array<float, 4> ghostRespawn = {0.f, 0.f, 0.f, 0.f};

    EventQueue<ConsumedPellet>  consumedThisTick;       ///< Buffered collectible removals for the current tick.
    EventQueue<EatenGhostEvent> eatenGhostsThisTick;    ///< Buffered frightened-ghost score popups.
    EventQueue<GridTileUpdate>  gridTileUpdatesThisTick;  ///< Buffered server-authored tile mutations.
    EventQueue<EatenGhostEvent> fruitPopupsThisTick;    ///< Buffered bonus-fruit score popups.

    int  currentLevel_{1};
    int  initialStartingPellets_{0};
    bool fruit70Processed_{false};
    bool fruit170Processed_{false};

    // Random 100/200/400/800 (BonusFruit1..4) per spawn; re-seeded on `resetForNewMatch`.
    std::mt19937 bonusFruitRng_{std::random_device{}()};

    void updateFruitSpawns();

    // ── Scatter / Chase cycling ───────────────────────────────────────────
    // Phase durations (seconds): scatter, chase, scatter, chase, scatter, chase, scatter, then infinite chase.
    // Classic Pac-Man level-1 schedule.
    static constexpr float PHASE_SCHEDULE[] = {7.f, 20.f, 7.f, 20.f, 5.f, 20.f, 5.f};
    static constexpr int   PHASE_COUNT      = 7;
    GhostMode              ghostMode{GhostMode::Scatter};
    int                    phaseIndex{0};
    float                  phaseTimer{PHASE_SCHEDULE[0]};

    void updatePlayerRespawns(float dt);
    void updateGhostRespawns(float dt, float scaledTileSize, float scale);
    void updateGhostExiting(int idx, float dt, float scaledTileSize, float scale);
    void updateGhostMode(float dt);  // advance scatter/chase phase timer
};
