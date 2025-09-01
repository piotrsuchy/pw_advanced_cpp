#include "graphics/LevelRenderer.hpp"

#include <iostream>

#include "graphics/Constants.hpp"

LevelRenderer::LevelRenderer() {
    // Load textures
    if (!wallTexture.loadFromFile("assets/textures/block2.png")) {
        std::cerr << "Failed to load block2.png\n";
    } else {
        std::cout << "Wall texture loaded: " << wallTexture.getSize().x << "x" << wallTexture.getSize().y << "\n";
    }
    if (!pelletTexture.loadFromFile("assets/textures/dot.png")) {
        std::cerr << "Failed to load dot.png\n";
    } else {
        std::cout << "Pellet texture loaded: " << pelletTexture.getSize().x << "x" << pelletTexture.getSize().y << "\n";
    }
    if (!powerPelletTexture.loadFromFile("assets/textures/cherry.png")) {
        std::cerr << "Failed to load cherry.png\n";
    } else {
        std::cout << "Power pellet texture loaded: " << powerPelletTexture.getSize().x << "x"
                  << powerPelletTexture.getSize().y << "\n";
    }

    wallSprite.setTexture(wallTexture);
    pelletSprite.setTexture(pelletTexture);
    powerPelletSprite.setTexture(powerPelletTexture);

    // Scale sprites to tileSize
    auto scaleToTileSize = [this](sf::Sprite& sprite) {
        sf::Vector2u textureSize = sprite.getTexture()->getSize();
        sprite.setScale(static_cast<float>(tileSize) / textureSize.x, static_cast<float>(tileSize) / textureSize.y);
    };

    scaleToTileSize(wallSprite);
    scaleToTileSize(pelletSprite);
    scaleToTileSize(powerPelletSprite);
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

    std::cout << "Recalculated offset: window(" << windowWidth << "," << windowHeight << ") - level(" << totalWidth
              << "," << totalHeight << ") / 2 = (" << offsetX << "," << offsetY << ")" << std::endl;
    std::cout << "Using scaled tile size: " << scaledTileSize << " (scale: " << scale << ")" << std::endl;

    static bool firstDraw = true;
    if (firstDraw) {
        std::cout << "Window size: " << window.getSize().x << "x" << window.getSize().y << "\n";
        std::cout << "Level size (tiles): " << gridWidth << "x" << gridHeight << "\n";
        std::cout << "Original level size (pixels): " << gridWidth * tileSize << "x" << gridHeight * tileSize << "\n";
        std::cout << "Scaled level size (pixels): " << totalWidth << "x" << totalHeight << "\n";
        std::cout << "Original tile size: " << tileSize << "\n";
        std::cout << "Scaled tile size: " << scaledTileSize << "\n";
        std::cout << "Offset: " << offsetX << "," << offsetY << "\n";

        // Print the first few maze tiles to verify content
        std::cout << "Maze content (first 5x5):" << std::endl;
        for (int y = 0; y < std::min(5, level.getHeight()); ++y) {
            for (int x = 0; x < std::min(5, level.getWidth()); ++x) {
                std::cout << static_cast<int>(level.getTile(x, y)) << " ";
            }
            std::cout << std::endl;
        }

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
                default:
                    break;
            }

            if (sprite) {
                sprite->setScale(scale, scale);
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