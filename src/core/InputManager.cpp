#include "core/InputManager.hpp"

void InputManager::pollInputs(sf::RenderWindow& window) {
    currentDirection = Direction::None;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        currentDirection = Direction::Up;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        currentDirection = Direction::Down;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        currentDirection = Direction::Left;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        currentDirection = Direction::Right;
    }
}

Direction InputManager::getDirection() const {
    return currentDirection;
}