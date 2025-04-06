#pragma once
#include <SFML/Graphics.hpp>
#include "core/InputManager.hpp"

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
    sf::RectangleShape testShape; // Placeholder for any entity

    InputManager inputManager;
};
