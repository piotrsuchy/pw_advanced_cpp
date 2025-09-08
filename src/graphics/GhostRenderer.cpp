#include "graphics/GhostRenderer.hpp"

GhostRenderer::GhostRenderer() {
    body.setRadius(24.f);
    body.setOrigin(24.f, 24.f);
    body.setFillColor(sf::Color(255, 0, 0));
}

bool GhostRenderer::setDirectionalTextures(const std::string& up, const std::string& down, const std::string& left,
                                           const std::string& right) {
    bool ok = true;
    ok &= texUp.loadFromFile(up);
    ok &= texDown.loadFromFile(down);
    ok &= texLeft.loadFromFile(left);
    ok &= texRight.loadFromFile(right);
    if (ok) {
        hasTextures = true;
        sprite.setTexture(texLeft);
        sprite.setOrigin(texLeft.getSize().x / 2.f, texLeft.getSize().y / 2.f);
    }
    return ok;
}

void GhostRenderer::setBaseColor(const sf::Color& c) {
    body.setFillColor(c);
}

void GhostRenderer::setPosition(float x, float y) {
    if (hasTextures) sprite.setPosition(x, y);
    body.setPosition(x, y);
}

void GhostRenderer::setFrightened(bool f) {
    frightened = f;
}

void GhostRenderer::setFacing(Direction d) {
    facing = d;
}

void GhostRenderer::tick(float, float scale) {
    body.setScale(scale / 1.0f, scale / 1.0f);
    if (hasTextures) sprite.setScale(scale, scale);
}

void GhostRenderer::draw(sf::RenderWindow& window) {
    static sf::Texture fright1, fright2;
    static bool        frightLoaded = false;
    static float       frightTimer  = 0.f;

    if (!frightLoaded) {
        frightLoaded = fright1.loadFromFile("assets/textures/ghosts/ghosts_power_pellet_1.png") &&
                       fright2.loadFromFile("assets/textures/ghosts/ghosts_power_pellet_2.png");
    }

    if (hasTextures) {
        if (frightened && frightLoaded) {
            // Alternate 8 times per second
            frightTimer += 0.016f;
            const sf::Texture& ft = ((static_cast<int>(frightTimer * 8) % 2) == 0) ? fright1 : fright2;
            sprite.setTexture(ft, true);
            window.draw(sprite);
            return;
        }
        // Pick texture based on direction
        switch (facing) {
            case Direction::Up:
                sprite.setTexture(texUp, true);
                break;
            case Direction::Down:
                sprite.setTexture(texDown, true);
                break;
            case Direction::Left:
                sprite.setTexture(texLeft, true);
                break;
            case Direction::Right:
                sprite.setTexture(texRight, true);
                break;
            default:
                break;
        }
        window.draw(sprite);
        return;
    }

    if (frightened) {
        auto c = body.getFillColor();
        body.setFillColor(sf::Color(0, 0, 255));
        window.draw(body);
        body.setFillColor(c);
    } else {
        window.draw(body);
    }
}
