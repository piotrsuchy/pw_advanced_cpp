#pragma once
#include <SFML/Graphics.hpp>
#include "core/LevelManager.hpp"

class LevelRenderer
{
public:
    LevelRenderer();
    void draw(sf::RenderWindow &window, const LevelManager &level);

private:
    sf::Texture wallTexture, pelletTexture, powerPelletTexture;
    sf::Sprite wallSprite, pelletSprite, powerPelletSprite;
};