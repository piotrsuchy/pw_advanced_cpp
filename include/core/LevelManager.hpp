#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType { Empty, Wall, Pellet, PowerPellet };

class LevelManager {
public:
    void loadLevel(int levelNumber);
    void draw(sf::RenderWindow& window);
    bool collectPellet(sf::Vector2i pos);

private:
    std::vector<std::vector<TileType>> grid;
    sf::Texture wallTexture, pelletTexture, powerPelletTexture;
    sf::Sprite wallSprite, pelletSprite, powerPelletSprite;
    const int tileSize = 20;
};