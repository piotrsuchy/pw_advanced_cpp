// graphics/PacMan_gfx.cpp
#include "core/PacMan.hpp"

PacMan::PacMan()
{
    if (!texture.loadFromFile("assets/textures/pacman.png"))
        sprite.setTextureRect({0, 0, 64, 64});
    else
        sprite.setTexture(texture);

    sprite.setColor(sf::Color::Yellow);
    sprite.setOrigin(32.f, 32.f);   // centre of 64×64
    sprite.setScale(1.f, 1.f);      // actual scale set each frame
    sprite.setPosition(position);   // initial pos
}

void PacMan::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}
