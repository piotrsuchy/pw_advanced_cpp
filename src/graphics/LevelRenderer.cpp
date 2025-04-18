#include "graphics/LevelRenderer.hpp"

LevelRenderer::LevelRenderer() {
    // Load textures from the existing pacman.png
    wallTexture.loadFromFile("assets/textures/pacman.png");
    pelletTexture.loadFromFile("assets/textures/pacman.png");
    powerPelletTexture.loadFromFile("assets/textures/pacman.png");

    wallSprite.setTexture(wallTexture);
    pelletSprite.setTexture(pelletTexture);
    powerPelletSprite.setTexture(powerPelletTexture);

    // Set texture rectangles for each sprite type
    wallSprite.setTextureRect({0, 0, 32, 32});
    pelletSprite.setTextureRect({32, 0, 32, 32});
    powerPelletSprite.setTextureRect({64, 0, 32, 32});

    // Scale sprites to match tile size
    float scale = static_cast<float>(tileSize) / 32.0f;
    wallSprite.setScale(scale, scale);
    pelletSprite.setScale(scale, scale);
    powerPelletSprite.setScale(scale, scale);
}

void LevelRenderer::draw(sf::RenderWindow& window, const LevelManager& level) {
    // Calculate the offset to center the maze
    float offsetX = (window.getSize().x - level.getWidth() * tileSize) / 2.0f;
    float offsetY = (window.getSize().y - level.getHeight() * tileSize) / 2.0f;

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
                sprite->setPosition(offsetX + x * tileSize, offsetY + y * tileSize);
                window.draw(*sprite);
            }
        }
    }
}