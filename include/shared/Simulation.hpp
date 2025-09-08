#pragma once

#include <vector>

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"
#include "shared/GhostLogic.hpp"
#include "shared/PacmanLogic.hpp"

struct PlayerStateView {
    Vec2      position;
    Direction facing;
    int       score{0};
    bool      powered{false};
    float     powerTimeLeft{0.f};
};

enum class ScoreEvent { Pellet, PowerPellet, Ghost, Cherry };

static constexpr int POINTS_PELLET       = 10;
static constexpr int POINTS_POWER_PELLET = 50;
static constexpr int POINTS_GHOST        = 200;
static constexpr int POINTS_CHERRY       = 100;  // classic Pac-Man fruit value for Cherry

class Simulation {
   public:
    Simulation();

    void initLevel(int levelNumber);

    // Set desired direction for player index 0 or 1
    void setDesired(int playerIndex, Direction d);

    // Fixed-step update; scale/tiles come from renderer conventions for now
    void step(float dt, float scaledTileSize, float scale);

    // Accessors
    PlayerStateView     getPlayerState(int playerIndex) const;
    const LevelManager& getLevel() const {
        return level;
    }
    // Ghost access
    Vec2      getGhostPosition(int ghostIndex) const;
    Direction getGhostFacing(int ghostIndex) const;
    bool      isGhostActive(int ghostIndex) const;

    struct ConsumedPellet {
        int      x;
        int      y;
        TileType type;
    };
    // Moves consumed pellets from the last step into 'out' and clears the internal buffer
    void drainConsumed(std::vector<ConsumedPellet>& out);

    // Award points for a scoring event (e.g., ghost eaten)
    void award(int playerIndex, ScoreEvent eventType);
    struct EatenGhostEvent {
        float x;
        float y;
        int   points;
    };
    void drainEatenGhosts(std::vector<EatenGhostEvent>& out);

   private:
    LevelManager level;
    PacmanLogic  players[2];
    Blinky       blinky;
    Pinky        pinky;
    Inky         inky;
    Clyde        clyde;
    bool         initializedPositions{false};

    // Scoring and power status
    int   score[2]              = {0, 0};
    float powerTimer[2]         = {0.f, 0.f};
    int   frightenedEatCount[2] = {0, 0};

    // Ghost home (respawn) position in world coords
    float ghostHomeX{0.f};
    float ghostHomeY{0.f};

    std::vector<ConsumedPellet>  consumedThisTick;
    std::vector<EatenGhostEvent> eatenGhostsThisTick;
    float                        ghostRespawn[4] = {0.f, 0.f, 0.f, 0.f};
};
