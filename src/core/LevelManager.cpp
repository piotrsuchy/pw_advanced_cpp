#include "core/LevelManager.hpp"

void LevelManager::loadLevel(int) {
    grid = {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,2,1,1,1,1,2,1},
        {1,2,1,0,1,2,1,0,0,1,2,1},
        {1,2,1,1,1,2,1,1,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,3,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    };

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

void LevelManager::draw(sf::RenderWindow& window) {
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid[y].size(); ++x) {
            sf::Sprite* sprite = nullptr;
            switch (grid[y][x]) {
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

bool LevelManager::collectPellet(sf::Vector2i pos) {
    if (grid[pos.y][pos.x] == TileType::Pellet || grid[pos.y][pos.x] == TileType::PowerPellet) {
        grid[pos.y][pos.x] = TileType::Empty;
        return true;
    }
    return false;
}