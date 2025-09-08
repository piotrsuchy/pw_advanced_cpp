#include "graphics/PacmanRenderer.hpp"

PacmanRenderer::PacmanRenderer() {
    loadedClosed = textureClosed.loadFromFile("assets/textures/pacman/pacman_closed.png");
    hasOpen      = textureOpen.loadFromFile("assets/textures/pacman/pacman_open.png");
    // Load death frames lazily on first use in setDeathTimeLeft to avoid startup cost
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

    if (deathTimeLeft > 0.f) {
        // 8 frames over 3 seconds
        float total   = 3.0f;
        int   frames  = 8;
        float elapsed = total - deathTimeLeft;
        int   idx     = std::min(frames - 1, std::max(0, (int)std::floor((elapsed / total) * frames)));
        if (!deathLoaded) {
            // selected some of the better frames for the dying animation
            const char* paths[8] = {"assets/textures/pacman/dying_2.png",  "assets/textures/pacman/dying_3.png",
                                    "assets/textures/pacman/dying_4.png",  "assets/textures/pacman/dying_6.png",
                                    "assets/textures/pacman/dying_8.png",  "assets/textures/pacman/dying_9.png",
                                    "assets/textures/pacman/dying_10.png", "assets/textures/pacman/dying_11.png"};
            bool        ok       = true;
            for (int i = 0; i < frames; ++i) ok &= deathFrames[i].loadFromFile(paths[i]);
            deathLoaded = ok;
        }
        if (deathLoaded) {
            sprite.setTexture(deathFrames[idx], true);
        } else {
            sprite.setTexture(textureClosed, true);
        }
        // Do not animate mouth while dying
        return;
    }

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

void PacmanRenderer::setDeathTimeLeft(float t) {
    deathTimeLeft = t;
}
