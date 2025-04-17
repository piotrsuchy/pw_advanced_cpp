#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"

PacMan::PacMan() {
    if (!texture.loadFromFile("assets/textures/pacman.png")) {
        sprite.setTextureRect(sf::IntRect(0, 0, 32, 32)); // fallback rectangle
    } else {
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    }

    sprite.setColor(sf::Color::Yellow);
    sprite.setOrigin(16.f, 16.f);  // Center of 32x32 sprite

    // Scale sprite to match tile size
    float scale = 20.0f / 32.0f;  // tileSize / textureSize
    sprite.setScale(scale, scale);

    // Start position will be set by GameController
    position = sf::Vector2f(400.f, 300.f);
    sprite.setPosition(position);
}

void PacMan::handleInput(Direction dir) {
    direction = dir;
}

void PacMan::update(float deltaTime, LevelManager& level) {
    sf::Vector2f movement(0.f, 0.f);
    sf::Vector2f nextPosition = position;

    switch (direction) {
        case Direction::Up: nextPosition.y -= speed * deltaTime; break;
        case Direction::Down: nextPosition.y += speed * deltaTime; break;
        case Direction::Left: nextPosition.x -= speed * deltaTime; break;
        case Direction::Right: nextPosition.x += speed * deltaTime; break;
        default: break;
    }

    // Convert screen coordinates to grid coordinates
    float offsetX = (800.f - level.getWidth() * 20.f) / 2.0f;  // window width - maze width
    float offsetY = (600.f - level.getHeight() * 20.f) / 2.0f;  // window height - maze height

    int tileX = static_cast<int>((nextPosition.x - offsetX) / 20.f);
    int tileY = static_cast<int>((nextPosition.y - offsetY) / 20.f);

    if (tileX >= 0 && tileY >= 0 && tileX < level.getWidth() && tileY < level.getHeight()) {
        if (level.getTile(tileX, tileY) != TileType::Wall) {
            position = nextPosition;
            sprite.setPosition(position);
            level.collectPellet(tileX, tileY);
        }
    }
}

void PacMan::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}