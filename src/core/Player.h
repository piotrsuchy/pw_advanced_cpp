#ifndef PACMAN_PLAYER_H
#define PACMAN_PLAYER_H

#include "Entity.h"
#include <string>

/**
 * @brief Class representing a player in the game
 *
 * The Player class handles player-specific behavior including movement,
 * scoring, and power-up states.
 */
class Player : public Entity
{
public:
    /**
     * @brief Construct a new Player
     * @param x Initial x position
     * @param y Initial y position
     * @param name Player's name
     */
    Player(float x, float y, const std::string &name);

    /**
     * @brief Update player state
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * @brief Handle collision with other entities
     * @param other The entity this player collided with
     */
    void onCollision(Entity *other) override;

    /**
     * @brief Add points to player's score
     * @param points Points to add
     */
    void addScore(int points);

    /**
     * @brief Get player's current score
     * @return Current score
     */
    int getScore() const { return score_; }

    /**
     * @brief Get player's name
     * @return Player name
     */
    const std::string &getName() const { return name_; }

    /**
     * @brief Activate power-up mode
     * @param duration Duration of the power-up in seconds
     */
    void activatePowerUp(float duration);

private:
    std::string name_;         ///< Player's name
    int score_;                ///< Current score
    bool isPoweredUp_;         ///< Power-up state
    float powerUpTimeLeft_;    ///< Remaining power-up time
    bool isCollidingWithWall_; ///< Flag indicating wall collision
};

#endif // PACMAN_PLAYER_H