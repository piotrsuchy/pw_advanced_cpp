#include "shared/PacmanLogic.hpp"

#include <cmath>

void PacmanLogic::update(float dt, LevelManager& level, float scaledTileSize, float scale) {
    float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    if (direction == Direction::None && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    }

    if (aligned(scaledTileSize, offX, offY) && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
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
    constexpr float eps = 2.f;
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
    if (curX < 0 || curY < 0 || curX >= lvl.getWidth() || curY >= lvl.getHeight()) return false;
    return lvl.getTile(curX, curY) != TileType::Wall;
}
