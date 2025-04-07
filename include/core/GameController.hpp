#pragma once
#include <SFML/Graphics.hpp>
#include "core/InputManager.hpp"
#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"

LevelManager level;

class GameController {
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
};
