#pragma once

#include <array>
#include <memory>
#include <vector>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"
#include "shared/GhostLogic.hpp"
#include "shared/InteractionResolver.hpp"
#include "shared/Player.hpp"

struct PlayerStateView {
    Vec2      position;
    Direction facing;
    int       score{0};
    bool      powered{false};
    float     powerTimeLeft{0.f};
    int       livesLeft{3};
    float     deathTimeLeft{0.f};
};

enum class ScoreEvent { Pellet, PowerPellet, Ghost, Cherry };

static constexpr int POINTS_PELLET       = 10;
static constexpr int POINTS_POWER_PELLET = 50;
static constexpr int POINTS_CHERRY       = 100;

class Simulation {
   public:
    Simulation();

    void initLevel(int levelNumber);

    // Set desired direction for player index 0 or 1
    void setDesired(int playerIndex, Direction d);

    // Fixed-step update
    void step(float dt, float scaledTileSize, float scale);

    // Accessors
    PlayerStateView     getPlayerState(int playerIndex) const;
    const LevelManager& getLevel() const {
        return level;
    }

    // Ghost access by index [0=Blinky, 1=Pinky, 2=Inky, 3=Clyde]
    Vec2      getGhostPosition(int ghostIndex) const;
    Direction getGhostFacing(int ghostIndex) const;
    bool      isGhostActive(int ghostIndex) const;
    bool      isGhostFrightened(int ghostIndex) const;

    struct ConsumedPellet {
        int      x;
        int      y;
        TileType type;
    };
    void drainConsumed(std::vector<ConsumedPellet>& out);

    struct EatenGhostEvent {
        float x;
        float y;
        int   points;
    };
    void drainEatenGhosts(std::vector<EatenGhostEvent>& out);

    bool isGameOver() const {
        return gameOver;
    }
    bool isLevelComplete() const {
        return levelComplete;
    }

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

    std::vector<ConsumedPellet>  consumedThisTick;
    std::vector<EatenGhostEvent> eatenGhostsThisTick;

    bool gameOver{false};
    bool levelComplete{false};

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
