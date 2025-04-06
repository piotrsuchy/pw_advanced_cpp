#include "core/GameController.hpp"

GameController::GameController()
    : window(sf::VideoMode(800, 600), "Pac-Man Clone") {
    window.setFramerateLimit(60);
    
    // Just a placeholder visual object
    testShape.setSize(sf::Vector2f(50.f, 50.f));
    testShape.setFillColor(sf::Color::Yellow);
    testShape.setPosition(375.f, 275.f); // Center-ish
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
    if (dir != Direction::None) {
        // Temporary test behavior
        sf::Vector2f movement(0.f, 0.f);
        switch (dir) {
            case Direction::Up: movement.y -= 100 * deltaTime; break;
            case Direction::Down: movement.y += 100 * deltaTime; break;
            case Direction::Left: movement.x -= 100 * deltaTime; break;
            case Direction::Right: movement.x += 100 * deltaTime; break;
            default: break;
        }
        testShape.move(movement);
    }

}

void GameController::render() {
    window.clear(sf::Color::Black);
    window.draw(testShape); // draw placeholder
    window.display();
}
