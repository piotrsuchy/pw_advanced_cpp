#pragma once

#include "core/LevelManager.hpp"
#include "shared/GameTypes.hpp"
#include "shared/PacmanLogic.hpp"

struct PlayerStateView {
    Vec2      position;
    Direction facing;
    int       score{0};
    bool      powered{false};
    float     powerTimeLeft{0.f};
};

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

   private:
    LevelManager level;
    PacmanLogic  players[2];
    bool         initializedPositions{false};

    // Scoring and power status
    int   score[2]      = {0, 0};
    float powerTimer[2] = {0.f, 0.f};
};
