#pragma once

#include <SFML/Graphics.hpp>

#include "shared/GameTypes.hpp"

class GhostRenderer {
   public:
    GhostRenderer();

    void setPosition(float x, float y);
    void setFrightened(bool f);
    void setBaseColor(const sf::Color& c);
    void setFacing(Direction d);
    void tick(float dt, float scale);
    void draw(sf::RenderWindow& window);

    // Optional: provide directional textures; if set, sprite rendering is used
    bool setDirectionalTextures(const std::string& up, const std::string& down, const std::string& left,
                                const std::string& right);

   private:
    // Fallback simple shape
    sf::CircleShape body;
    bool            frightened{false};

    // Textured sprite path
    bool        hasTextures{false};
    Direction   facing{Direction::Left};
    sf::Texture texUp, texDown, texLeft, texRight;
    sf::Sprite  sprite;
};
