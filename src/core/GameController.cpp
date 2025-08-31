#include "core/GameController.hpp"
#include "graphics/Constants.hpp"
#include "core/PacMan.hpp"
#include "core/DebugUtils.hpp"
#include <iostream>

GameController::GameController()
    : window(sf::VideoMode(800, 600), "Pac-Man Clone")
{
    window.setFramerateLimit(60);
    level.loadLevel(1);

    // Pacman will be positioned in the update method after first render
    // when we know the actual scaled tile size
}

void GameController::run()
{
    bool firstFrame = true;

    while (window.isOpen())
    {
        std::cout << "DEBUG: Position: " << pacman.getPosition() << "\n";
        std::cout << "DEBUG: Direction: " << pacman.getDirection() << "\n";
        float deltaTime = clock.restart().asSeconds();
        processEvents();

        if (firstFrame)
        {
            // Size the maze to fit in the window
            int totalWidth = level.getWidth() * tileSize;
            int totalHeight = level.getHeight() * tileSize;

            float scaleX = static_cast<float>(window.getSize().x) / totalWidth;
            float scaleY = static_cast<float>(window.getSize().y) / totalHeight;
            float scale = std::min(scaleX, scaleY) * 0.9f;

            scaledTileSize = tileSize * scale;

            // Get offsets for centering
            float offsetX = (window.getSize().x - level.getWidth() * scaledTileSize) / 2.0f;
            float offsetY = (window.getSize().y - level.getHeight() * scaledTileSize) / 2.0f;

            // Position Pac-Man in the center of the first row where there's a pellet
            for (int x = 0; x < level.getWidth(); ++x)
            {
                if (level.getTile(x, 1) == TileType::Pellet)
                {
                    pacman.setPosition(offsetX + x * scaledTileSize + scaledTileSize / 2,
                                       offsetY + scaledTileSize + scaledTileSize / 2);
                    break;
                }
            }

            firstFrame = false;
        }

        update(deltaTime);
        render();
    }
}

void GameController::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        // forward everything else to the input system
        inputManager.handleEvent(event);
    }
}

void GameController::update(float deltaTime)
{
    // at most one key press per frame
    if (Direction d = inputManager.popQueuedDirection(); d != Direction::None)
        pacman.handleInput(d);
    // Update pacman with the scaled tile size
    int totalWidth = level.getWidth() * tileSize;
    int totalHeight = level.getHeight() * tileSize;

    float scaleX = static_cast<float>(window.getSize().x) / totalWidth;
    float scaleY = static_cast<float>(window.getSize().y) / totalHeight;
    float scale = std::min(scaleX, scaleY) * 0.9f;

    // Pass this scale to PacMan's update method
    pacman.update(deltaTime, level, scaledTileSize, scale);
}

void GameController::render()
{
    window.clear(sf::Color::Black);
    levelRenderer.draw(window, level); // use renderer, not LevelManager
    pacman.draw(window);
    window.display();
}