#include "graphics/PacmanRenderer.hpp"

PacmanRenderer::PacmanRenderer() {
    loadedClosed = textureClosed.loadFromFile("assets/textures/pacman/pacman_closed.png");
    hasOpen      = textureOpen.loadFromFile("assets/textures/pacman/pacman_open.png");
    if (loadedClosed) {
        sprite.setTexture(textureClosed, true);
        auto size = textureClosed.getSize();
        sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    }
}

void PacmanRenderer::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void PacmanRenderer::setFacing(Direction d) {
    float rotation = 0.f;
    switch (d) {
        case Direction::Right:
            rotation = 0.f;
            break;
        case Direction::Left:
            rotation = 180.f;
            break;
        case Direction::Up:
            rotation = 270.f;
            break;
        case Direction::Down:
            rotation = 90.f;
            break;
        default:
            break;
    }
    sprite.setRotation(rotation);
}

void PacmanRenderer::tick(float dt, bool isMoving, float scale) {
    if (loadedClosed) {
        auto size = sprite.getTexture()->getSize();
        sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    }
    sprite.setScale(scale, scale);

    if (isMoving) {
        timer += dt;
        if (timer >= interval) {
            timer     = 0.f;
            mouthOpen = !mouthOpen;
            if (hasOpen && mouthOpen) {
                sprite.setTexture(textureOpen, true);
            } else {
                sprite.setTexture(textureClosed, true);
            }
        }
    } else {
        timer     = 0.f;
        mouthOpen = false;
        sprite.setTexture(textureClosed, true);
    }
}

void PacmanRenderer::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
