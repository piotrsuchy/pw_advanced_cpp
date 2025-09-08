#pragma once
#include <SFML/Graphics.hpp>

#include "core/LevelManager.hpp"

class LevelRenderer {
   public:
    LevelRenderer();
    void draw(sf::RenderWindow& window, const LevelManager& level);

   private:
    sf::Texture wallTexture, pelletTexture, powerPelletTexture, cherryTexture;
    sf::Sprite  wallSprite, pelletSprite, powerPelletSprite, cherrySprite;
};