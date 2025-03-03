#ifndef PACMAN_WALL_H
#define PACMAN_WALL_H

#include "Entity.h"

/**
 * @brief Class representing a wall in the game maze
 *
 * Walls are static entities that block player movement and define
 * the maze structure.
 */
class Wall : public Entity
{
public:
    /**
     * @brief Construct a new Wall
     * @param x Initial x position
     * @param y Initial y position
     */
    Wall(float x, float y);

    /**
     * @brief Update wall state (no-op for static walls)
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * @brief Handle collision with other entities
     * @param other The entity this wall collided with
     */
    void onCollision(Entity *other) override;
};

#endif // PACMAN_WALL_H