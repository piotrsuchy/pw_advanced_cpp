#pragma once
#include <SFML/Graphics.hpp>
#include <array>

#include "core/LevelManager.hpp"

/**
 * @brief Renders the tile-based maze and collectibles for the current level.
 */
class LevelRenderer {
   public:
    /**
     * @brief Loads textures and creates the level renderer.
     */
    LevelRenderer();

    /**
     * @brief Draws the current level state into the target window.
     *
     * @param window Render target.
     * @param level Level state to render.
     */
    void draw(sf::RenderWindow& window, const LevelManager& level);

   private:
    sf::Texture                wallTexture, pelletTexture, powerPelletTexture;
    std::array<sf::Texture, 4> bonusFruitTexture{};
    sf::Sprite                 wallSprite, pelletSprite, powerPelletSprite;
    std::array<sf::Sprite, 4>  bonusFruitSprite{};
};
