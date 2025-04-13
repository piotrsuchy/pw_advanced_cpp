#include "graphics/LevelRenderer.hpp"

LevelRenderer::LevelRenderer() {
    wallTexture.loadFromFile("assets/textures/pacman.png");
    pelletTexture.loadFromFile("assets/textures/pacman.png");
    powerPelletTexture.loadFromFile("assets/textures/pacman.png");

    wallSprite.setTexture(wallTexture);
    pelletSprite.setTexture(pelletTexture);
    powerPelletSprite.setTexture(powerPelletTexture);

    wallSprite.setTextureRect({0, 0, 20, 20});
    pelletSprite.setTextureRect({20, 0, 5, 5});
    powerPelletSprite.setTextureRect({25, 0, 10, 10});
}

void LevelRenderer::draw(sf::RenderWindow& window, const LevelManager& level) {
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            sf::Sprite* sprite = nullptr;
            switch (level.getTile(x, y)) {
                case TileType::Wall: sprite = &wallSprite; break;
                case TileType::Pellet: sprite = &pelletSprite; break;
                case TileType::PowerPellet: sprite = &powerPelletSprite; break;
                default: break;
            }
            if (sprite) {
                sprite->setPosition(x * tileSize, y * tileSize);
                window.draw(*sprite);
            }
        }
    }
}