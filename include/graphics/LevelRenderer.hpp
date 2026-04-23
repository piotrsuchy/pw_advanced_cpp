#pragma once
#include <SFML/Graphics.hpp>
#include <array>

#include "core/LevelManager.hpp"

class LevelRenderer {
   public:
    LevelRenderer();
    void draw(sf::RenderWindow& window, const LevelManager& level);

   private:
    sf::Texture                wallTexture, pelletTexture, powerPelletTexture;
    std::array<sf::Texture, 4> bonusFruitTexture{};
    sf::Sprite                 wallSprite, pelletSprite, powerPelletSprite;
    std::array<sf::Sprite, 4>  bonusFruitSprite{};
};
