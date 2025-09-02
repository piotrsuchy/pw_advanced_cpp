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
    Direction d = queuedDir;
    queuedDir   = Direction::None;
    return d;
}