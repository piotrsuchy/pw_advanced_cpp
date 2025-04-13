#include "core/GameController.hpp"

GameController::GameController()
    : window(sf::VideoMode(800, 600), "Pac-Man Clone") {
    window.setFramerateLimit(60);
    level.loadLevel(1);
}

void GameController::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void GameController::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GameController::update(float deltaTime) {
    inputManager.pollInputs(window);

    Direction dir = inputManager.getDirection();
    pacman.handleInput(dir);
    pacman.update(deltaTime);
}

void GameController::render() {
    window.clear(sf::Color::Black);
    // window.draw(testShape); // draw placeholder
    levelRenderer.draw(window, level);
    pacman.draw(window); // draw placeholder
    window.display();
}
