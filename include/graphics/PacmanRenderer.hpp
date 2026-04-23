#pragma once

#include <SFML/Graphics.hpp>

#include "shared/GameTypes.hpp"

/**
 * @brief Draws Pac-Man using animated textures and death frames.
 */
class PacmanRenderer {
   public:
    /**
     * @brief Loads Pac-Man textures and initializes animation state.
     */
    PacmanRenderer();

    /**
     * @brief Sets the screen-space position of Pac-Man.
     *
     * @param x X coordinate in pixels.
     * @param y Y coordinate in pixels.
     */
    void setPosition(float x, float y);

    /**
     * @brief Sets the direction used to orient the sprite.
     *
     * @param d Facing direction.
     */
    void setFacing(Direction d);

    /**
     * @brief Advances mouth animation and scaling state.
     *
     * @param dt Elapsed time in seconds.
     * @param isMoving `true` when Pac-Man is currently moving.
     * @param scale World scale factor.
     */
    void tick(float dt, bool isMoving, float scale);

    /**
     * @brief Draws Pac-Man into the target render window.
     *
     * @param window Render target.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Sets the remaining death animation time.
     *
     * @param t Remaining death animation time in seconds.
     */
    void setDeathTimeLeft(float t);

   private:
    // Textures and sprite
    sf::Texture textureOpen;
    sf::Texture textureClosed;
    bool        hasOpen{false};
    bool        loadedClosed{false};
    sf::Sprite  sprite;
    // death frames
    bool        deathLoaded{false};
    sf::Texture deathFrames[11];
    float       deathTimeLeft{0.f};

    // Animation state
    bool  mouthOpen{false};
    float timer{0.f};
    float interval{0.1f};
};
