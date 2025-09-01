#include "core/PacMan.hpp"

#include <iostream>

#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp"  // For tileSize

void PacMan::handleInput(Direction dir) {
    if (dir != Direction::None) {
        desiredDirection = dir;
    }
}

void PacMan::update(float dt, LevelManager& level, float scaledTileSize, float scale) {
    // screen‑to‑grid helpers
    float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    // 0) if we are not moving, try to start moving straight away
    if (direction == Direction::None && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    }

    // 1) normal queued-turn logic (needs alignment)
    if (aligned(scaledTileSize, offX, offY) && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    } else {
        std::cout << "DEBUG: Can't move or is not aligned" << "\n";
        std::cout << "DEBUG: CAN MOVE: " << canMove(desiredDirection, level, scaledTileSize, offX, offY) << "\n";
        std::cout << "DEBUG: ALIGNED: " << aligned(scaledTileSize, offX, offY) << "\n";
    }

    // compute tentative next position along current heading
    sf::Vector2f next = position;
    float        v    = speed * scale;
    switch (direction) {
        case Direction::Up:
            next.y -= v * dt;
            break;
        case Direction::Down:
            next.y += v * dt;
            break;
        case Direction::Left:
            next.x -= v * dt;
            break;
        case Direction::Right:
            next.x += v * dt;
            break;
        default:
            break;
    }

    // grid coordinate of the tile we would step into
    int tileX = static_cast<int>(std::floor((next.x - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((next.y - offY) / scaledTileSize));

    // if wall in front - stop and snap to centre else move forward
    if (level.getTile(tileX, tileY) != TileType::Wall) {
        position = next;
    } else {  // hit a wall
        direction        = Direction::None;
        desiredDirection = Direction::None;
    }

    // keep the perpendicular axis perfectly centred
    int curX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
    int curY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));

    float cx = offX + curX * scaledTileSize + scaledTileSize / 2.f;
    float cy = offY + curY * scaledTileSize + scaledTileSize / 2.f;

    if (direction == Direction::Left || direction == Direction::Right)
        position.y = cy;
    else if (direction == Direction::Up || direction == Direction::Down)
        position.x = cx;

    // Animation: toggle mouth when moving; keep closed when stopped
    bool isMoving        = (direction != Direction::None);
    auto applyMouthFrame = [this]() {
        if (useSeparateTextures) {
            if (isMouthOpen && hasOpenTexture) {
                sprite.setTexture(textureOpen, true);
            } else {
                sprite.setTexture(textureClosed, true);
            }
            auto size = sprite.getTexture()->getSize();
            sprite.setOrigin(size.x / 2.f, size.y / 2.f);
        } else {
            sprite.setTextureRect(isMouthOpen ? frameOpen : frameClosed);
            sprite.setOrigin(32.f, 32.f);
        }
    };

    if (isMoving) {
        animationTimer += dt;
        if (animationTimer >= animationInterval) {
            animationTimer = 0.f;
            isMouthOpen    = !isMouthOpen;
            applyMouthFrame();
        }
    } else {
        isMouthOpen    = false;
        animationTimer = 0.f;
        applyMouthFrame();
    }

    // Orient sprite to current direction
    float rotationDegrees = 0.f;
    switch (direction) {
        case Direction::Right:
            rotationDegrees = 0.f;
            break;
        case Direction::Left:
            rotationDegrees = 180.f;
            break;
        case Direction::Up:
            rotationDegrees = 270.f;
            break;
        case Direction::Down:
            rotationDegrees = 90.f;
            break;
        default:
            break;
    }
    sprite.setRotation(rotationDegrees);

    // book‑keeping and drawing stuff
    sprite.setScale(scale, scale);
    sprite.setPosition(position);
    level.collectPellet(curX, curY);
}

// helpers
bool PacMan::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 2.f;  // half a pixel tolerance

    float cx = std::fmod(position.x - offX, tile);
    float cy = std::fmod(position.y - offY, tile);

    return (std::abs(cx - tile / 2.f) < eps) && (std::abs(cy - tile / 2.f) < eps);
}

bool PacMan::canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) {
    if (dir == Direction::None) return false;

    int curX = static_cast<int>(std::floor((position.x - offX) / tile));
    int curY = static_cast<int>(std::floor((position.y - offY) / tile));

    switch (dir) {
        case Direction::Up:
            --curY;
            break;
        case Direction::Down:
            ++curY;
            break;
        case Direction::Left:
            --curX;
            break;
        case Direction::Right:
            ++curX;
            break;
        default:
            break;
    }
    // can't go out of the board
    if (curX < 0 || curY < 0 || curX >= lvl.getWidth() || curY >= lvl.getHeight()) return false;

    return lvl.getTile(curX, curY) != TileType::Wall;
}

Direction PacMan::getDirection() {
    return PacMan::direction;
}

sf::Vector2f PacMan::getPosition() {
    return PacMan::position;
}