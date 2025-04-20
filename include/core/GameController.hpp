#pragma once
#include <SFML/Graphics.hpp>
#include "core/InputManager.hpp"
#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp"

class GameController
{
public:
    GameController();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    sf::Clock clock;
    // sf::RectangleShape testShape; // Placeholder
    PacMan pacman;

    InputManager inputManager;

    LevelManager level;
    LevelRenderer levelRenderer;
    float scaledTileSize = 0.0f; // Store the scaled tile size
};
