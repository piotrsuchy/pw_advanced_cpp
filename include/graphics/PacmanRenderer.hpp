#pragma once

#include <SFML/Graphics.hpp>

#include "shared/GameTypes.hpp"

class PacmanRenderer {
   public:
    PacmanRenderer();

    void setPosition(float x, float y);
    void setFacing(Direction d);
    void tick(float dt, bool isMoving, float scale);
    void draw(sf::RenderWindow& window);
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
