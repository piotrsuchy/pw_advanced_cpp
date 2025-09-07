#include "shared/GhostLogic.hpp"

#include <algorithm>
#include <cmath>

#include "shared/PacmanLogic.hpp"  // for Vec2 forward-decl definition

struct Vec2Impl {
    float x;
    float y;
};

Vec2 GhostBase::getPosition() const {
    Vec2 v;
    v.x = positionX;
    v.y = positionY;
    return v;
}

bool GhostBase::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 2.f;
    float           cx  = std::fmod(positionX - offX, tile);
    float           cy  = std::fmod(positionY - offY, tile);
    return (std::abs(cx - tile / 2.f) < eps) && (std::abs(cy - tile / 2.f) < eps);
}

bool GhostBase::canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) const {
    if (dir == Direction::None) return false;
    int curX = static_cast<int>(std::floor((positionX - offX) / tile));
    int curY = static_cast<int>(std::floor((positionY - offY) / tile));
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

void GhostBase::update(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                       Direction pac0Facing, Vec2 pac1Pos) {
    const float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    const float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    if (frightenedTimer > 0.f) frightenedTimer = std::max(0.f, frightenedTimer - dt);

    // Choose a direction when at intersections or standing still
    if (direction == Direction::None || aligned(scaledTileSize, offX, offY)) {
        auto [tx, ty] = targetTile(level, scaledTileSize, pac0Pos, pac0Facing, pac1Pos);
        int cx        = static_cast<int>(std::floor((positionX - offX) / scaledTileSize));
        int cy        = static_cast<int>(std::floor((positionY - offY) / scaledTileSize));

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
            if (nx < 0 || ny < 0 || nx >= level.getWidth() || ny >= level.getHeight()) continue;
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

    float v = (isFrightened() ? (speed * 0.6f) : speed) * scale;
    switch (direction) {
        case Direction::Up:
            positionY -= v * dt;
            break;
        case Direction::Down:
            positionY += v * dt;
            break;
        case Direction::Left:
            positionX -= v * dt;
            break;
        case Direction::Right:
            positionX += v * dt;
            break;
        default:
            break;
    }

    // Clamp to non-walls
    int tileX = static_cast<int>(std::floor((positionX - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((positionY - offY) / scaledTileSize));
    if (level.getTile(tileX, tileY) == TileType::Wall) {
        // back off and stop
        switch (direction) {
            case Direction::Up:
                positionY += v * dt;
                break;
            case Direction::Down:
                positionY -= v * dt;
                break;
            case Direction::Left:
                positionX += v * dt;
                break;
            case Direction::Right:
                positionX -= v * dt;
                break;
            default:
                break;
        }
        direction = Direction::None;
    }
}

// Simple target implementations
std::pair<int, int> Blinky::targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2) const {
    // Aim directly at player 0
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    return {tx, ty};
}

std::pair<int, int> Pinky::targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing,
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

std::pair<int, int> Inky::targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction,
                                     Vec2 pac1Pos) const {
    // Aim roughly between players
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    float mx   = (pac0Pos.x + pac1Pos.x) * 0.5f;
    float my   = (pac0Pos.y + pac1Pos.y) * 0.5f;
    int   tx   = static_cast<int>(std::floor((mx - offX) / tile));
    int   ty   = static_cast<int>(std::floor((my - offY) / tile));
    return {std::clamp(tx, 0, level.getWidth() - 1), std::clamp(ty, 0, level.getHeight() - 1)};
}

std::pair<int, int> Clyde::targetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2) const {
    // If close, run to a corner; else chase
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    int   cx   = static_cast<int>(std::floor((positionX - offX) / tile));
    int   cy   = static_cast<int>(std::floor((positionY - offY) / tile));
    int   dist = std::abs(tx - cx) + std::abs(ty - cy);
    if (dist < 6) return {1, level.getHeight() - 2};
    return {tx, ty};
}
