#pragma once

#include <SFML/Graphics.hpp>

#include "shared/GameTypes.hpp"

/**
 * @brief Draws one ghost using either textures or a simple fallback shape.
 */
class GhostRenderer {
   public:
    /**
     * @brief Creates a ghost renderer with default visual state.
     */
    GhostRenderer();

    /**
     * @brief Sets the screen-space position of the ghost.
     *
     * @param x X coordinate in pixels.
     * @param y Y coordinate in pixels.
     */
    void setPosition(float x, float y);

    /**
     * @brief Enables or disables frightened coloring for the ghost.
     *
     * @param f `true` to draw the frightened state.
     */
    void setFrightened(bool f);

    /**
     * @brief Enables the end-of-frightened flashing state.
     *
     * @param on `true` to alternate blue/white visuals near frightened end.
     */
    void setFrightenedEndFlash(bool on);

    /**
     * @brief Sets the shared phase used for end-flash alternation.
     *
     * @param t Monotonic phase time in seconds.
     */
    void setFrightenFlashPhase(float t);

    /**
     * @brief Sets the base body color used outside frightened mode.
     *
     * @param c Base ghost color.
     */
    void setBaseColor(const sf::Color& c);

    /**
     * @brief Sets the facing direction used for textured rendering.
     *
     * @param d Current facing direction.
     */
    void setFacing(Direction d);

    /**
     * @brief Advances any time-based animation state.
     *
     * @param dt Elapsed time in seconds.
     * @param scale World scale factor.
     */
    void tick(float dt, float scale);

    /**
     * @brief Draws the ghost into the target render window.
     *
     * @param window Render target.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Loads directional textures for sprite-based ghost rendering.
     *
     * @param up Path to the up-facing texture.
     * @param down Path to the down-facing texture.
     * @param left Path to the left-facing texture.
     * @param right Path to the right-facing texture.
     * @return `true` if all textures loaded successfully.
     */
    bool setDirectionalTextures(const std::string& up, const std::string& down, const std::string& left,
                                const std::string& right);

   private:
    // Fallback simple shape
    sf::CircleShape body;
    bool            frightened{false};
    bool            frightenedEndFlash{false};
    float           frightenFlashPhaseT_{0.f};

    // Textured sprite path
    bool        hasTextures{false};
    Direction   facing{Direction::Left};
    sf::Texture texUp, texDown, texLeft, texRight;
    sf::Sprite  sprite;
};
