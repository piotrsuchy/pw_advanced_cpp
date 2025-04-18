#include "core/GameController.hpp"

GameController::GameController()
    : window(sf::VideoMode(800, 600), "Pac-Man Clone") {
    window.setFramerateLimit(60);
    level.loadLevel(1);

    // Calculate the center position for Pac-Man
    float offsetX = (window.getSize().x - level.getWidth() * 20.f) / 2.0f;
    float offsetY = (window.getSize().y - level.getHeight() * 20.f) / 2.0f;
    
    // Position Pac-Man in the center of the first row where there's a pellet
    for (int x = 0; x < level.getWidth(); ++x) {
        if (level.getTile(x, 1) == TileType::Pellet) {
            pacman.setPosition(offsetX + x * 20.f + 10.f, offsetY + 20.f + 10.f);
            break;
        }
    }
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
    pacman.update(deltaTime, level);
}

void GameController::render() {
    window.clear(sf::Color::Black);
    levelRenderer.draw(window, level); // use renderer, not LevelManager
    pacman.draw(window);
    window.display();
}