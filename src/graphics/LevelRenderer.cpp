#include "graphics/LevelRenderer.hpp"

#include <iostream>
#include <string>

#include "graphics/Constants.hpp"

namespace {
    // Fruit PNGs are 32×32; walls/pellets use 64×64 (`tileSize`). Compensate in world space before window scale.
    constexpr float kBonusFruitAssetScale = 2.f;
}  // namespace

LevelRenderer::LevelRenderer() {
    // Load textures
    if (!wallTexture.loadFromFile("assets/textures/map/block.png")) {
        std::cerr << "Failed to load block.png\n";
    } else {
        std::cout << "Wall texture loaded: " << wallTexture.getSize().x << "x" << wallTexture.getSize().y << "\n";
    }
    if (!pelletTexture.loadFromFile("assets/textures/map/pellet.png")) {
        std::cerr << "Failed to load pellet.png\n";
    } else {
        std::cout << "Pellet texture loaded: " << pelletTexture.getSize().x << "x" << pelletTexture.getSize().y << "\n";
    }
    if (!powerPelletTexture.loadFromFile("assets/textures/map/power_pellet.png")) {
        std::cerr << "Failed to load power_pellet.png\n";
    } else {
        std::cout << "Power pellet texture loaded: " << powerPelletTexture.getSize().x << "x"
                  << powerPelletTexture.getSize().y << "\n";
    }
    for (int i = 0; i < 4; ++i) {
        const std::string path = "assets/textures/map/fruit-" + std::to_string(i + 1) + ".png";
        if (!bonusFruitTexture[static_cast<std::size_t>(i)].loadFromFile(path)) {
            std::cerr << "Failed to load " << path << "\n";
        } else {
            std::cout << "Bonus fruit " << (i + 1) << " texture loaded\n";
        }
        bonusFruitSprite[static_cast<std::size_t>(i)].setTexture(bonusFruitTexture[static_cast<std::size_t>(i)]);
    }

    wallSprite.setTexture(wallTexture);
    pelletSprite.setTexture(pelletTexture);
    powerPelletSprite.setTexture(powerPelletTexture);
}

void LevelRenderer::draw(sf::RenderWindow& window, const LevelManager& level) {
    // Calculate the size of the level
    int gridWidth   = level.getWidth();
    int gridHeight  = level.getHeight();
    int totalWidth  = gridWidth * tileSize;
    int totalHeight = gridHeight * tileSize;

    // Get window dimensions
    int windowWidth  = static_cast<int>(window.getSize().x);
    int windowHeight = static_cast<int>(window.getSize().y);

    // Scale down the tile size if needed to fit the maze in the window
    float scaleX = 1.0f, scaleY = 1.0f;
    float scaledTileSize = tileSize;

    if (totalWidth > windowWidth) {
        scaleX = static_cast<float>(windowWidth) / totalWidth;
    }
    if (totalHeight > windowHeight) {
        scaleY = static_cast<float>(windowHeight) / totalHeight;
    }

    // Use the smaller scale to maintain aspect ratio
    float scale    = std::min(scaleX, scaleY) * 0.9f;  // Leave a 10% margin
    scaledTileSize = tileSize * scale;

    // Recalculate the total size with the scaled tile size
    totalWidth  = gridWidth * scaledTileSize;
    totalHeight = gridHeight * scaledTileSize;

    // Calculate the offset to center the maze
    float offsetX = (windowWidth - totalWidth) / 2.0f;
    float offsetY = (windowHeight - totalHeight) / 2.0f;

    static bool firstDraw = true;
    if (firstDraw) {
        std::cout << "Window size: " << window.getSize().x << "x" << window.getSize().y << "\n";
        std::cout << "Level size (tiles): " << gridWidth << "x" << gridHeight << "\n";
        std::cout << "Original level size (pixels): " << gridWidth * tileSize << "x" << gridHeight * tileSize << "\n";
        std::cout << "Scaled level size (pixels): " << totalWidth << "x" << totalHeight << "\n";
        std::cout << "Original tile size: " << tileSize << "\n";
        std::cout << "Scaled tile size: " << scaledTileSize << "\n";
        std::cout << "Offset: " << offsetX << "," << offsetY << "\n";

        firstDraw = false;
    }

    // Draw background
    sf::RectangleShape background(sf::Vector2f(totalWidth, totalHeight));
    background.setPosition(offsetX, offsetY);
    background.setFillColor(sf::Color::Black);  // Dark gray
    window.draw(background);

    // Draw grid cells with different colors for debugging
    int tilesDrawn = 0;
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            TileType tileType = level.getTile(x, y);

            // Debug rectangle to show tile boundaries
            sf::RectangleShape tileRect(sf::Vector2f(scaledTileSize, scaledTileSize));
            // tileRect.setPosition(offsetX + x * scaledTileSize, offsetY + y * scaledTileSize);
            // tileRect.setOutlineColor(sf::Color::White);
            // tileRect.setOutlineThickness(1);

            tileRect.setFillColor(sf::Color::Black);
            window.draw(tileRect);

            // Try to draw sprites
            sf::Sprite* sprite = nullptr;
            switch (tileType) {
                case TileType::Wall:
                    sprite = &wallSprite;
                    break;
                case TileType::Pellet:
                    sprite = &pelletSprite;
                    break;
                case TileType::PowerPellet:
                    sprite = &powerPelletSprite;
                    break;
                case TileType::BonusFruit1:
                    sprite = &bonusFruitSprite[0];
                    break;
                case TileType::BonusFruit2:
                    sprite = &bonusFruitSprite[1];
                    break;
                case TileType::BonusFruit3:
                    sprite = &bonusFruitSprite[2];
                    break;
                case TileType::BonusFruit4:
                    sprite = &bonusFruitSprite[3];
                    break;
                default:
                    break;
            }

            if (sprite) {
                const bool  bonusFruit = tileType >= TileType::BonusFruit1 && tileType <= TileType::BonusFruit4;
                const float s          = scale * (bonusFruit ? kBonusFruitAssetScale : 1.f);
                sprite->setScale(s, s);
                sprite->setPosition(offsetX + x * scaledTileSize, offsetY + y * scaledTileSize);
                window.draw(*sprite);
                tilesDrawn++;
            }
        }
    }

    static bool reportedTiles = false;
    if (!reportedTiles) {
        std::cout << "Total tiles drawn: " << tilesDrawn << std::endl;
        reportedTiles = true;
    }
}
