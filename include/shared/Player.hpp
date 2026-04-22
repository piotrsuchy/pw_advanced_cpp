#pragma once

#include "shared/GameTypes.hpp"
#include "shared/PacmanLogic.hpp"

// Represents a connected player's game state (score, lives, their Pacman logic).
// The Match/Simulation owns a fixed array of Players and coordinates them.
class Player {
   public:
    explicit Player(int id) : playerId(id) {}

    int getId() const {
        return playerId;
    }

    // Score
    int getScore() const {
        return score;
    }
    void addScore(int pts) {
        score += pts;
    }

    // Lives
    int getLives() const {
        return lives;
    }
    void loseLife() {
        if (lives > 0) --lives;
    }
    bool isAlive() const {
        return lives > 0;
    }

    // Death respawn timer
    float getDeathTimer() const {
        return deathTimer;
    }
    void setDeathTimer(float t) {
        deathTimer = t;
    }
    bool isDying() const {
        return deathTimer > 0.f;
    }
    void tickDeathTimer(float dt) {
        if (deathTimer > 0.f) {
            deathTimer -= dt;
            if (deathTimer < 0.f) deathTimer = 0.f;
        }
    }

    // Power pellet
    float getPowerTimer() const {
        return powerTimer;
    }
    void setPowerTimer(float t) {
        powerTimer = t;
    }
    bool isPowered() const {
        return powerTimer > 0.f;
    }
    void tickPowerTimer(float dt) {
        if (powerTimer > 0.f) {
            powerTimer -= dt;
            if (powerTimer < 0.f) powerTimer = 0.f;
        }
    }

    // Frightened eat combo count (resets on new power pellet activation)
    int getFrightenedEatCount() const {
        return frightenedEatCount;
    }
    void incrementFrightenedEatCount() {
        ++frightenedEatCount;
    }
    void resetFrightenedEatCount() {
        frightenedEatCount = 0;
    }

    // Spawn position (set once by Simulation on init)
    void setSpawn(float x, float y) {
        spawnX = x;
        spawnY = y;
    }
    float getSpawnX() const {
        return spawnX;
    }
    float getSpawnY() const {
        return spawnY;
    }

    // The actual Pac-Man movement logic
    PacmanLogic& getPacman() {
        return pacman;
    }
    const PacmanLogic& getPacman() const {
        return pacman;
    }

   private:
    int   playerId{0};
    int   score{0};
    int   lives{3};
    float deathTimer{0.f};
    float powerTimer{0.f};
    int   frightenedEatCount{0};
    float spawnX{0.f};
    float spawnY{0.f};

    PacmanLogic pacman;
};
