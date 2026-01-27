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
    }
    // 2) Cornering: allow perpendicular turns within CORNER_TOLERANCE of tile center
    else if (canCornerTurn(desiredDirection, scaledTileSize, offX, offY, level)) {
        // Snap to the perpendicular lane center before turning
        int   curX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
        int   curY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));
        float cx   = offX + curX * scaledTileSize + scaledTileSize / 2.f;
        float cy   = offY + curY * scaledTileSize + scaledTileSize / 2.f;

        // Snap the axis we're about to move along
        if (desiredDirection == Direction::Left || desiredDirection == Direction::Right) {
            position.y = cy;
        } else if (desiredDirection == Direction::Up || desiredDirection == Direction::Down) {
            position.x = cx;
        }
        direction = desiredDirection;
    }

    // remember last non-None facing direction
    if (direction != Direction::None) {
        facingDirection = direction;
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

    // Orient sprite to current or last facing direction
    float     rotationDegrees = 0.f;
    float     scaleX          = scale;
    float     scaleY          = scale;
    Direction orientDir       = (direction == Direction::None ? facingDirection : direction);
    switch (orientDir) {
        case Direction::Right:
            rotationDegrees = 0.f;
            scaleX          = scale;  // normal
            break;
        case Direction::Left:
            rotationDegrees = 0.f;     // mirror horizontally instead of rotating 180° to keep eye on top
            scaleX          = -scale;  // flip X
            break;
        case Direction::Up:
            rotationDegrees = 270.f;  // face up
            scaleX          = scale;
            break;
        case Direction::Down:
            rotationDegrees = 90.f;  // face down
            scaleX          = scale;
            break;
        default:
            break;
    }
    sprite.setRotation(rotationDegrees);

    // book‑keeping and drawing stuff
    sprite.setScale(scaleX, scaleY);
    sprite.setPosition(position);
    level.collectPellet(curX, curY);
}

// helpers
bool PacMan::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 6.f;  // increased for better responsiveness

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

bool PacMan::isPerpendicular(Direction a, Direction b) const {
    if (a == Direction::None || b == Direction::None) return false;
    bool aHorizontal = (a == Direction::Left || a == Direction::Right);
    bool bHorizontal = (b == Direction::Left || b == Direction::Right);
    return aHorizontal != bHorizontal;
}

bool PacMan::canCornerTurn(Direction newDir, float tile, float offX, float offY, const LevelManager& lvl) const {
    // Only allow cornering for perpendicular turns
    if (!isPerpendicular(newDir, direction)) return false;
    if (newDir == Direction::None) return false;

    // Calculate current tile and center
    int   curX = static_cast<int>(std::floor((position.x - offX) / tile));
    int   curY = static_cast<int>(std::floor((position.y - offY) / tile));
    float cx   = offX + curX * tile + tile / 2.f;
    float cy   = offY + curY * tile + tile / 2.f;

    // Check distance along the current movement axis
    float distToCenter = 0.f;
    if (direction == Direction::Left || direction == Direction::Right) {
        distToCenter = std::abs(position.x - cx);
    } else if (direction == Direction::Up || direction == Direction::Down) {
        distToCenter = std::abs(position.y - cy);
    }

    // Must be within corner tolerance
    if (distToCenter > CORNER_TOLERANCE) return false;

    // Check if target tile is not a wall
    int targetX = curX;
    int targetY = curY;
    switch (newDir) {
        case Direction::Up:
            --targetY;
            break;
        case Direction::Down:
            ++targetY;
            break;
        case Direction::Left:
            --targetX;
            break;
        case Direction::Right:
            ++targetX;
            break;
        default:
            return false;
    }

    if (targetX < 0 || targetY < 0 || targetX >= lvl.getWidth() || targetY >= lvl.getHeight()) return false;

    return lvl.getTile(targetX, targetY) != TileType::Wall;
}