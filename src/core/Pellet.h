#ifndef PACMAN_PELLET_H
#define PACMAN_PELLET_H

#include "Entity.h"

/**
 * @brief Class representing a collectible pellet
 *
 * Pellets are items that players can collect to score points.
 * There are two types: regular pellets and power pellets.
 */
class Pellet : public Entity
{
public:
    /**
     * @brief Construct a new Pellet
     * @param x Initial x position
     * @param y Initial y position
     * @param isPowerPellet Whether this is a power pellet
     */
    Pellet(float x, float y, bool isPowerPellet = false);

    /**
     * @brief Update pellet state
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * @brief Handle collision with other entities
     * @param other The entity this pellet collided with
     */
    void onCollision(Entity *other) override;

    /**
     * @brief Check if this is a power pellet
     * @return true if power pellet, false if regular pellet
     */
    bool isPowerPellet() const { return isPowerPellet_; }

    /**
     * @brief Get the point value of this pellet
     * @return Points awarded for collecting this pellet
     */
    int getPoints() const { return isPowerPellet_ ? 50 : 10; }

private:
    bool isPowerPellet_; ///< Whether this is a power pellet
};

#endif // PACMAN_PELLET_H