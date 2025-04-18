#include <iostream>
#include "graphics/LevelRenderer.hpp"

LevelRenderer::LevelRenderer() {
    // Load textures
    if (!wallTexture.loadFromFile("assets/textures/wall.png"))
        std::cerr << "Failed to load wall.png\n";
    if (!pelletTexture.loadFromFile("assets/textures/dot.png"))
        std::cerr << "Failed to load dot.png\n";
    if (!powerPelletTexture.loadFromFile("assets/textures/cherry.png"))
        std::cerr << "Failed to load cherry.png\n";

    wallSprite.setTexture(wallTexture);
    pelletSprite.setTexture(pelletTexture);
    powerPelletSprite.setTexture(powerPelletTexture);

    // No cropping needed — just scale from 64x64 → tileSize
    auto scaleFrom64 = [this](sf::Sprite& s) {
        s.setScale(static_cast<float>(tileSize) / 64.f,
                   static_cast<float>(tileSize) / 64.f);
    };

    scaleFrom64(wallSprite);
    scaleFrom64(pelletSprite);
    scaleFrom64(powerPelletSprite);
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