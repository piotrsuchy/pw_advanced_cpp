// graphics/PacMan_gfx.cpp
#include "core/PacMan.hpp"

PacMan::PacMan() {
    // Try spritesheet first
    bool sheetLoaded = texture.loadFromFile("assets/textures/pacman_spritesheet.png");

    // Try optional separate textures
    bool closedLoaded   = textureClosed.loadFromFile("assets/textures/pacman_closed.png");
    bool openLoaded     = textureOpen.loadFromFile("assets/textures/pacman_open.png");
    useSeparateTextures = closedLoaded;  // require closed; open is optional
    hasOpenTexture      = openLoaded;

    if (useSeparateTextures) {
        sprite.setTexture(textureClosed);
        // scale will be set in update; origin to image center
        auto size = textureClosed.getSize();
        sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    } else if (sheetLoaded) {
        sprite.setTexture(texture);
        sprite.setTextureRect(frameClosed);
        sprite.setOrigin(32.f, 32.f);  // centre of 64×64 in sheet
    } else {
        // fallback: no textures, keep default rect
        sprite.setTextureRect({0, 0, 64, 64});
        sprite.setOrigin(32.f, 32.f);
    }

    sprite.setColor(sf::Color::Yellow);
    sprite.setScale(1.f, 1.f);     // actual scale set each frame
    sprite.setPosition(position);  // initial pos
}

void PacMan::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
