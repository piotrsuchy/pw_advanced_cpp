#include "shared/PacmanLogic.hpp"

#include <cmath>

void PacmanLogic::update(float dt, LevelManager& level, float scaledTileSize, float scale) {
    float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    // If not moving, try to start moving
    if (direction == Direction::None && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    }

    // Standard aligned turn
    if (aligned(scaledTileSize, offX, offY) && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    }
    // Cornering: allow perpendicular turns within CORNER_TOLERANCE of tile center
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

    if (direction != Direction::None) facingDirection = direction;

    Vec2  next = position;
    float v    = speed * scale;
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

    int tileX = static_cast<int>(std::floor((next.x - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((next.y - offY) / scaledTileSize));
    if (level.getTile(tileX, tileY) != TileType::Wall) {
        position = next;
        // Tunnel wrap: if position has crossed the horizontal boundary, teleport to other side
        const float worldLeft  = offX;
        const float worldRight = offX + level.getWidth() * scaledTileSize;
        if (position.x < worldLeft) position.x += level.getWidth() * scaledTileSize;
        if (position.x >= worldRight) position.x -= level.getWidth() * scaledTileSize;
    } else {
        direction        = Direction::None;
        desiredDirection = Direction::None;
    }

    int   curX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
    int   curY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));
    float cx   = offX + curX * scaledTileSize + scaledTileSize / 2.f;
    float cy   = offY + curY * scaledTileSize + scaledTileSize / 2.f;
    if (direction == Direction::Left || direction == Direction::Right)
        position.y = cy;
    else if (direction == Direction::Up || direction == Direction::Down)
        position.x = cx;

    // Pellet collection is handled authoritatively in Simulation
}

bool PacmanLogic::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 6.f;  // increased for better responsiveness
    float           cx  = std::fmod(position.x - offX, tile);
    float           cy  = std::fmod(position.y - offY, tile);
    return (std::abs(cx - tile / 2.f) < eps) && (std::abs(cy - tile / 2.f) < eps);
}

bool PacmanLogic::canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) {
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
    // Only y is a hard boundary; x wraps via LevelManager::getTile()
    if (curY < 0 || curY >= lvl.getHeight()) return false;
    return lvl.getTile(curX, curY) != TileType::Wall;
}

bool PacmanLogic::isPerpendicular(Direction a, Direction b) const {
    if (a == Direction::None || b == Direction::None) return false;
    bool aHorizontal = (a == Direction::Left || a == Direction::Right);
    bool bHorizontal = (b == Direction::Left || b == Direction::Right);
    return aHorizontal != bHorizontal;
}

bool PacmanLogic::canCornerTurn(Direction newDir, float tile, float offX, float offY, const LevelManager& lvl) const {
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
