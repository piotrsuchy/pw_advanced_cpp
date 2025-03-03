#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H

#include <SFML/System/Vector2.hpp>
#include <memory>

/**
 * @brief Base class for all game entities
 *
 * This class serves as the foundation for all objects in the game,
 * implementing common functionality such as position, movement, and collision detection.
 */
class Entity
{
public:
    /**
     * @brief Constructor for Entity
     * @param x Initial x position
     * @param y Initial y position
     */
    Entity(float x, float y);

    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes
     */
    virtual ~Entity() = default;

    /**
     * @brief Updates the entity's state
     * @param deltaTime Time elapsed since last update
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Handles collision with another entity
     * @param other The entity this one collided with
     */
    virtual void onCollision(Entity *other) = 0;

    /**
     * @brief Get the current position
     * @return Current position as SFML Vector2f
     */
    const sf::Vector2f &getPosition() const { return position_; }

    /**
     * @brief Set the entity's position
     * @param x New x position
     * @param y New y position
     */
    void setPosition(float x, float y);

    /**
     * @brief Get the entity's velocity
     * @return Current velocity as SFML Vector2f
     */
    const sf::Vector2f &getVelocity() const { return velocity_; }

    /**
     * @brief Set the entity's velocity
     * @param vx Velocity in x direction
     * @param vy Velocity in y direction
     */
    void setVelocity(float vx, float vy);

    /**
     * @brief Check if the entity is active
     * @return true if active, false if inactive
     */
    bool isActive() const { return active_; }

protected:
    sf::Vector2f position_; ///< Current position
    sf::Vector2f velocity_; ///< Current velocity
    bool active_;           ///< Whether the entity is active in the game
};

#endif // PACMAN_ENTITY_H