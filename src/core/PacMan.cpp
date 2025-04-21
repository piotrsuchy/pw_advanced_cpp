#include <iostream>
#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp" // For tileSize

void PacMan::handleInput(Direction dir)
{
    if (dir != Direction::None) {
        desiredDirection = dir;
    }
}

void PacMan::update(float dt, LevelManager& level,
    float scaledTileSize, float scale)
{
    // screen‑to‑grid helpers
    float offX = (800.f - level.getWidth()  * scaledTileSize) / 2.f;
    float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    // try to honour the queued turn when we’re exactly on a tile centre
    if (aligned(scaledTileSize, offX, offY) &&
        canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
        direction = desiredDirection;
    } else {
        std::cout << "DEBUG: Can't move or is not aligned" << "\n";
        std::cout << "DEBUG: CAN MOVE: " << canMove(desiredDirection, level, scaledTileSize, offX, offY) << "\n";
        std::cout << "DEBUG: ALIGNED: " << aligned(scaledTileSize, offX, offY) << "\n";
    }

    // compute tentative next position along current heading
    sf::Vector2f next = position;
    float v = speed * scale;
    switch (direction) {
        case Direction::Up:    next.y -= v * dt; break;
        case Direction::Down:  next.y += v * dt; break;
        case Direction::Left:  next.x -= v * dt; break;
        case Direction::Right: next.x += v * dt; break;
        default: break;
    }

    // grid coordinate of the tile we would step into 
    int tileX = static_cast<int>(std::floor((next.x - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((next.y - offY) / scaledTileSize));

    // if wall in front - stop and snap to centre else move forward
    if (level.getTile(tileX, tileY) != TileType::Wall) {
        position = next;
    } else {
        direction = Direction::None;
        desiredDirection = Direction::None;
    }

    // keep the perpendicular axis perfectly centred
    int curX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
    int curY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));

    float cx  = offX + curX * scaledTileSize + scaledTileSize / 2.f;
    float cy  = offY + curY * scaledTileSize + scaledTileSize / 2.f;

    if (direction == Direction::Left || direction == Direction::Right)
        position.y = cy;
    else if (direction == Direction::Up || direction == Direction::Down)
        position.x = cx;

    // book‑keeping and drawing stuff
    sprite.setScale(scale, scale);
    sprite.setPosition(position);
    level.collectPellet(curX, curY);
}

// helpers
bool PacMan::aligned(float tile, float offX, float offY) const
{
    constexpr float eps = 2.f; // half a pixel tolerance

    float cx = std::fmod(position.x - offX, tile);
    float cy = std::fmod(position.y - offY, tile);

    return (std::abs(cx - tile / 2.f) < eps) &&
    (std::abs(cy - tile / 2.f) < eps);
}

bool PacMan::canMove(Direction dir, const LevelManager& lvl,
    float tile, float offX, float offY)
{
    if (dir == Direction::None) return false;

    int curX = static_cast<int>(std::floor((position.x - offX) / tile));
    int curY = static_cast<int>(std::floor((position.y - offY) / tile));

    switch (dir) {
        case Direction::Up:    --curY; break;
        case Direction::Down:  ++curY; break;
        case Direction::Left:  --curX; break;
        case Direction::Right: ++curX; break;
        default: break;
    }
    // can't go out of the board
    if (curX < 0 || curY < 0 ||
        curX >= lvl.getWidth() || curY >= lvl.getHeight())
        return false;

    bool ok = lvl.getTile(curX, curY) != TileType::Wall;

    // if the requested tile is a wall, forget the request
    // so the player can press a different key straight away
    // instead of being "stuck"
    if (!ok) {
        std::cout << "requested tile is a wall" << "/n";
        desiredDirection = Direction::None;
    }
    return ok;
}

Direction PacMan::getDirection() {
    return PacMan::direction;
}

sf::Vector2f PacMan::getPosition() {
    return PacMan::position;
}