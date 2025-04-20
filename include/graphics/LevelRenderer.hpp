#pragma once
#include <SFML/Graphics.hpp>
#include "core/LevelManager.hpp"

class LevelRenderer
{
public:
    LevelRenderer();
    static constexpr int tileSize = 64; // Standardize to 64x64

    void draw(sf::RenderWindow &window, const LevelManager &level);

private:
    sf::Texture wallTexture, pelletTexture, powerPelletTexture;
    sf::Sprite wallSprite, pelletSprite, powerPelletSprite;
};