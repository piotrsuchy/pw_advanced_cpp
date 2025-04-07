#include "core/PacMan.hpp"

PacMan::PacMan() {
    sprite.setColor(sf::Color::Yellow);
    sprite.setOrigin(16.f, 16.f);
    sprite.setPosition(400.f, 300.f);

    if (!texture.loadFromFile("assets/textures/pacman.png")) {
        sprite.setTextureRect(sf::IntRect(0, 0, 32, 32)); // fallback rectangle
    } else {
        sprite.setTexture(texture);
    }

    position = sprite.getPosition();
}

void PacMan::handleInput(Direction dir) {
    direction = dir;
}

void PacMan::update(float deltaTime) {
    sf::Vector2f movement(0.f, 0.f);

    switch (direction) {
        case Direction::Up: movement.y -= speed * deltaTime; break;
        case Direction::Down: movement.y += speed * deltaTime; break;
        case Direction::Left: movement.x -= speed * deltaTime; break;
        case Direction::Right: movement.x += speed * deltaTime; break;
        default: break;
    }

    position += movement;
    sprite.setPosition(position);
}

void PacMan::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}