#include "core/InputManager.hpp"

void InputManager::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        switch (e.key.code) {
            case sf::Keyboard::Up:
                queuedDir = Direction::Up;
                break;
            case sf::Keyboard::Down:
                queuedDir = Direction::Down;
                break;
            case sf::Keyboard::Left:
                queuedDir = Direction::Left;
                break;
            case sf::Keyboard::Right:
                queuedDir = Direction::Right;
                break;
            default:
                break;
        }
    }
}

Direction InputManager::popQueuedDirection() {
    // Persistent: direction stays until new key is pressed
    return queuedDir;
}

Direction InputManager::getDesiredDirection() const {
    return queuedDir;
}

void InputManager::clearQueuedDirection() {
    queuedDir = Direction::None;
}