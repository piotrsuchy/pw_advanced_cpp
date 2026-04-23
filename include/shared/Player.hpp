#pragma once

#include "shared/GameTypes.hpp"
#include "shared/PacmanLogic.hpp"

/**
 * @brief Stores gameplay state for one connected player.
 *
 * A `Player` keeps score, life counters, timers, spawn information, and the
 * owned `PacmanLogic` instance for movement.
 */
class Player {
   public:
    /**
     * @brief Creates a player with the given logical identifier.
     *
     * @param id Player slot identifier.
     */
    explicit Player(int id) : playerId(id) {}

    /**
     * @brief Returns the player's fixed slot identifier.
     *
     * @return Player id.
     */
    int getId() const {
        return playerId;
    }

    /**
     * @brief Returns the player's current score.
     *
     * @return Current score.
     */
    int getScore() const {
        return score;
    }

    /**
     * @brief Adds points to the player's score.
     *
     * @param pts Number of points to add.
     */
    void addScore(int pts) {
        score += pts;
    }

    /**
     * @brief Returns the number of lives remaining.
     *
     * @return Remaining lives.
     */
    int getLives() const {
        return lives;
    }

    /**
     * @brief Consumes one life if the player still has any left.
     */
    void loseLife() {
        if (lives > 0) --lives;
    }

    /**
     * @brief Checks whether the player still has at least one life left.
     *
     * @return `true` if the player is still alive.
     */
    bool isAlive() const {
        return lives > 0;
    }

    /**
     * @brief Returns the remaining death/respawn delay.
     *
     * @return Remaining delay in seconds.
     */
    float getDeathTimer() const {
        return deathTimer;
    }

    /**
     * @brief Sets the death/respawn delay.
     *
     * @param t New timer value in seconds.
     */
    void setDeathTimer(float t) {
        deathTimer = t;
    }

    /**
     * @brief Checks whether the player is currently in the death delay state.
     *
     * @return `true` if the death timer is active.
     */
    bool isDying() const {
        return deathTimer > 0.f;
    }

    /**
     * @brief Advances the death timer toward zero.
     *
     * @param dt Elapsed time in seconds.
     */
    void tickDeathTimer(float dt) {
        if (deathTimer > 0.f) {
            deathTimer -= dt;
            if (deathTimer < 0.f) deathTimer = 0.f;
        }
    }

    /**
     * @brief Returns the remaining power-pellet timer.
     *
     * @return Remaining power time in seconds.
     */
    float getPowerTimer() const {
        return powerTimer;
    }

    /**
     * @brief Sets the active power-pellet timer.
     *
     * @param t New timer value in seconds.
     */
    void setPowerTimer(float t) {
        powerTimer = t;
    }

    /**
     * @brief Checks whether the player is currently powered up.
     *
     * @return `true` if the power timer is active.
     */
    bool isPowered() const {
        return powerTimer > 0.f;
    }

    /**
     * @brief Advances the power timer toward zero.
     *
     * @param dt Elapsed time in seconds.
     */
    void tickPowerTimer(float dt) {
        if (powerTimer > 0.f) {
            powerTimer -= dt;
            if (powerTimer < 0.f) powerTimer = 0.f;
        }
    }

    /**
     * @brief Returns the current frightened ghost combo count.
     *
     * @return Number of ghosts eaten during the current power window.
     */
    int getFrightenedEatCount() const {
        return frightenedEatCount;
    }

    /**
     * @brief Increments the frightened ghost combo count by one.
     */
    void incrementFrightenedEatCount() {
        ++frightenedEatCount;
    }

    /**
     * @brief Resets the frightened ghost combo count to zero.
     */
    void resetFrightenedEatCount() {
        frightenedEatCount = 0;
    }

    /**
     * @brief Stores the player's respawn position.
     *
     * @param x Spawn X coordinate.
     * @param y Spawn Y coordinate.
     */
    void setSpawn(float x, float y) {
        spawnX = x;
        spawnY = y;
    }

    /**
     * @brief Returns the stored spawn X coordinate.
     *
     * @return Spawn X position.
     */
    float getSpawnX() const {
        return spawnX;
    }

    /**
     * @brief Returns the stored spawn Y coordinate.
     *
     * @return Spawn Y position.
     */
    float getSpawnY() const {
        return spawnY;
    }

    /**
     * @brief Returns mutable access to the owned Pac-Man movement logic.
     *
     * @return Mutable `PacmanLogic` instance.
     */
    PacmanLogic& getPacman() {
        return pacman;
    }

    /**
     * @brief Returns read-only access to the owned Pac-Man movement logic.
     *
     * @return Const reference to the `PacmanLogic` instance.
     */
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
