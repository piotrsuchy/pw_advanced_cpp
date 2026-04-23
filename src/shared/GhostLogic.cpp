#include "shared/GhostLogic.hpp"

#include <algorithm>
#include <cmath>

Ghost::Ghost(std::unique_ptr<IGhostAI> aiStrategy) : ai(std::move(aiStrategy)) {}

void Ghost::update(float dt) {
    // Basic update without context. The Simulation/Match will call updateLogic instead.
}

Vec2 Ghost::getPosition() const {
    return position;
}

void Ghost::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

Direction Ghost::getFacing() const {
    return direction;
}

void Ghost::setFrightened(float seconds) {
    frightenedTimer = seconds;
}

bool Ghost::isFrightened() const {
    return frightenedTimer > 0.f;
}

bool Ghost::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 2.f;
    float           cx  = std::fmod(position.x - offX, tile);
    float           cy  = std::fmod(position.y - offY, tile);
    return (std::abs(cx - tile / 2.f) < eps) && (std::abs(cy - tile / 2.f) < eps);
}

bool Ghost::canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) const {
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

void Ghost::updateLogic(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                        Direction pac0Facing, Vec2 pac1Pos) {
    const float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    const float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    if (frightenedTimer > 0.f) frightenedTimer = std::max(0.f, frightenedTimer - dt);

    // Choose a direction when at intersections or standing still
    if (direction == Direction::None || aligned(scaledTileSize, offX, offY)) {
        auto [tx, ty] = ai->getTargetTile(level, scaledTileSize, pac0Pos, pac0Facing, pac1Pos, position);
        int cx        = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
        int cy        = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));

        // Try the direction that reduces Manhattan distance; avoid reversing if possible
        Direction candidates[4] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};
        float     bestScore     = 1e9f;
        Direction bestDir       = Direction::None;
        for (Direction d : candidates) {
            if (direction != Direction::None) {
                if ((direction == Direction::Up && d == Direction::Down) ||
                    (direction == Direction::Down && d == Direction::Up) ||
                    (direction == Direction::Left && d == Direction::Right) ||
                    (direction == Direction::Right && d == Direction::Left)) {
                    continue;  // avoid immediate reverse
                }
            }
            int nx = cx, ny = cy;
            if (d == Direction::Up) --ny;
            if (d == Direction::Down) ++ny;
            if (d == Direction::Left) --nx;
            if (d == Direction::Right) ++nx;
            if (ny < 0 || ny >= level.getHeight()) continue;  // y is hard boundary
            // x wraps via getTile(); don't skip out-of-bounds nx
            if (level.getTile(nx, ny) == TileType::Wall) continue;
            float score = std::abs(tx - nx) + std::abs(ty - ny);
            if (score < bestScore) {
                bestScore = score;
                bestDir   = d;
            }
        }
        if (bestDir == Direction::None) {
            // fall back: try any open dir including reverse
            for (Direction d : candidates) {
                if (canMove(d, level, scaledTileSize, offX, offY)) {
                    bestDir = d;
                    break;
                }
            }
        }
        direction = bestDir;
    }

    // slow to 60% of normal speed
    float v = (isFrightened() ? (speed * 0.6f) : speed) * scale;
    switch (direction) {
        case Direction::Up:
            position.y -= v * dt;
            break;
        case Direction::Down:
            position.y += v * dt;
            break;
        case Direction::Left:
            position.x -= v * dt;
            break;
        case Direction::Right:
            position.x += v * dt;
            break;
        default:
            break;
    }

    // Clamp to non-walls
    int tileX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));
    if (level.getTile(tileX, tileY) == TileType::Wall) {
        // back off and stop
        switch (direction) {
            case Direction::Up:
                position.y += v * dt;
                break;
            case Direction::Down:
                position.y -= v * dt;
                break;
            case Direction::Left:
                position.x += v * dt;
                break;
            case Direction::Right:
                position.x -= v * dt;
                break;
            default:
                break;
        }
        direction = Direction::None;
    }

    // Tunnel wrap: teleport to opposite side when crossing horizontal boundary
    const float worldLeft  = offX;
    const float worldRight = offX + level.getWidth() * scaledTileSize;
    if (position.x < worldLeft) position.x += level.getWidth() * scaledTileSize;
    if (position.x >= worldRight) position.x -= level.getWidth() * scaledTileSize;
}

// Simple target implementations
std::pair<int, int> BlinkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                            Vec2) const {
    // Aim directly at player 0
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    return {tx, ty};
}

std::pair<int, int> PinkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing, Vec2,
                                           Vec2) const {
    // Aim four tiles ahead of player 0
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    int   dx = 0, dy = 0;
    if (facing == Direction::Up)
        dy = -4;
    else if (facing == Direction::Down)
        dy = 4;
    else if (facing == Direction::Left)
        dx = -4;
    else if (facing == Direction::Right)
        dx = 4;
    return {std::clamp(tx + dx, 0, level.getWidth() - 1), std::clamp(ty + dy, 0, level.getHeight() - 1)};
}

std::pair<int, int> InkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2 pac1Pos,
                                          Vec2) const {
    // Aim roughly between players
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    float mx   = (pac0Pos.x + pac1Pos.x) * 0.5f;
    float my   = (pac0Pos.y + pac1Pos.y) * 0.5f;
    int   tx   = static_cast<int>(std::floor((mx - offX) / tile));
    int   ty   = static_cast<int>(std::floor((my - offY) / tile));
    return {std::clamp(tx, 0, level.getWidth() - 1), std::clamp(ty, 0, level.getHeight() - 1)};
}

std::pair<int, int> ClydeAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                           Vec2 currentPos) const {
    // If close, run to a corner; else chase
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    int   cx   = static_cast<int>(std::floor((currentPos.x - offX) / tile));
    int   cy   = static_cast<int>(std::floor((currentPos.y - offY) / tile));
    int   dist = std::abs(tx - cx) + std::abs(ty - cy);
    if (dist < 6) return {1, level.getHeight() - 2};
    return {tx, ty};
}
