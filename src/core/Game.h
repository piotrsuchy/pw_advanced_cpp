#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include "../utils/Grid.h"
#include "Entity.h"
#include "Player.h"
#include "Wall.h"
#include "Pellet.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @brief Main game class managing game state and logic
 *
 * This class handles:
 * - Game initialization
 * - Entity management
 * - Game loop
 * - Collision detection
 * - Score tracking
 */
class Game
{
public:
    /**
     * @brief Construct a new Game
     * @param window Reference to the SFML window
     */
    Game(sf::RenderWindow &window);

    /**
     * @brief Initialize the game state
     */
    void initialize();

    /**
     * @brief Update game state
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);

    /**
     * @brief Render the game
     */
    void render();

    /**
     * @brief Handle input events
     * @param event SFML event to handle
     */
    void handleEvent(const sf::Event &event);

private:
    /**
     * @brief Load the maze layout
     */
    void loadMaze();

    /**
     * @brief Check for collisions between entities
     */
    void checkCollisions();

    sf::RenderWindow &window_;                      ///< Reference to the game window
    Grid<int> maze_;                                ///< Game maze layout
    std::vector<std::unique_ptr<Entity>> entities_; ///< All game entities
    std::vector<Player *> players_;                 ///< Pointers to player entities

    // Visual representations
    sf::CircleShape playerShape_;      ///< Shape for players
    sf::RectangleShape wallShape_;     ///< Shape for walls
    sf::CircleShape pelletShape_;      ///< Shape for regular pellets
    sf::CircleShape powerPelletShape_; ///< Shape for power pellets
};

#endif // PACMAN_GAME_H