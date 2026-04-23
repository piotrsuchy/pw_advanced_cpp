#include "shared/GhostLogic.hpp"

#include <algorithm>
#include <cmath>

// ── Ghost ────────────────────────────────────────────────────────────────────

Ghost::Ghost(std::unique_ptr<IGhostAI> aiStrategy) : ai(std::move(aiStrategy)) {}

void Ghost::update(float /*dt*/) {
    // Contextless update stub — Simulation calls updateLogic() instead.
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

void Ghost::reverseDirection() {
    switch (direction) {
        case Direction::Up:
            direction = Direction::Down;
            break;
        case Direction::Down:
            direction = Direction::Up;
            break;
        case Direction::Left:
            direction = Direction::Right;
            break;
        case Direction::Right:
            direction = Direction::Left;
            break;
        default:
            break;
    }
}

// Returns true when the ghost centre is within `eps` pixels of a tile centre.
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
    if (curY < 0 || curY >= lvl.getHeight()) return false;
    return lvl.getTile(curX, curY) != TileType::Wall;
}

void Ghost::updateLogic(float dt, const LevelManager& level, float scaledTileSize, float scale, Vec2 pac0Pos,
                        Direction pac0Facing, Vec2 pac1Pos, GhostMode mode, float cruiseElroyChaseSpeedMul) {
    const float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    const float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;

    if (frightenedTimer > 0.f) frightenedTimer = std::max(0.f, frightenedTimer - dt);

    // Choose a new direction when aligned to a tile centre (intersection).
    if (direction == Direction::None || aligned(scaledTileSize, offX, offY)) {
        // Determine target tile based on mode (frightened overrides).
        std::pair<int, int> target;
        if (frightenedTimer > 0.f) {
            // Frightened: use scatter corner as a crude "run away" target
            // (the direction-choice inversion below effectively runs away).
            target = ai->getScatterTile(level);
        } else if (mode == GhostMode::Scatter) {
            target = ai->getScatterTile(level);
        } else {
            target = ai->getTargetTile(level, scaledTileSize, pac0Pos, pac0Facing, pac1Pos, position);
        }
        auto [tx, ty] = target;

        int cx = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
        int cy = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));

        Direction candidates[4] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};
        float     bestScore     = 1e9f;
        Direction bestDir       = Direction::None;

        for (Direction d : candidates) {
            // No immediate 180° reverse (unless forced).
            if (direction != Direction::None) {
                if ((direction == Direction::Up && d == Direction::Down) ||
                    (direction == Direction::Down && d == Direction::Up) ||
                    (direction == Direction::Left && d == Direction::Right) ||
                    (direction == Direction::Right && d == Direction::Left)) {
                    continue;
                }
            }
            int nx = cx, ny = cy;
            if (d == Direction::Up) --ny;
            if (d == Direction::Down) ++ny;
            if (d == Direction::Left) --nx;
            if (d == Direction::Right) ++nx;
            if (ny < 0 || ny >= level.getHeight()) continue;
            if (level.getTile(nx, ny) == TileType::Wall) continue;

            // Frightened: maximise distance from scatter corner (run away).
            float score =
                isFrightened() ? -(std::abs(tx - nx) + std::abs(ty - ny)) : (std::abs(tx - nx) + std::abs(ty - ny));

            if (score < bestScore) {
                bestScore = score;
                bestDir   = d;
            }
        }

        // Fallback: allow any direction including reverse.
        if (bestDir == Direction::None) {
            for (Direction d : candidates) {
                if (canMove(d, level, scaledTileSize, offX, offY)) {
                    bestDir = d;
                    break;
                }
            }
        }
        direction = bestDir;
    }

    // Move — frightened ghosts run at 60% of normal speed; classic tunnel is slower still.
    const int  rowForTunnel = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));
    const bool inSideTunnel = (direction == Direction::Left || direction == Direction::Right) &&
                              level.isHorizontalGhostTunnelRow(rowForTunnel);
    // Pac-Man style: ghosts move at ~40% in the horizontal wrap tunnel; use 0.5 as a clear game feel.
    static constexpr float kTunnelGhostSpeedMul = 0.5f;
    float                  v                    = (isFrightened() ? speed * 0.6f : speed) * scale;
    if (inSideTunnel) v *= kTunnelGhostSpeedMul;
    v *= cruiseElroyChaseSpeedMul;
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

    // Push out of walls.
    int tileX = static_cast<int>(std::floor((position.x - offX) / scaledTileSize));
    int tileY = static_cast<int>(std::floor((position.y - offY) / scaledTileSize));  // after move
    if (level.getTile(tileX, tileY) == TileType::Wall) {
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

    // Horizontal tunnel wrap.
    const float worldLeft  = offX;
    const float worldRight = offX + level.getWidth() * scaledTileSize;
    if (position.x < worldLeft) position.x += level.getWidth() * scaledTileSize;
    if (position.x >= worldRight) position.x -= level.getWidth() * scaledTileSize;
}

// ── AI strategies ────────────────────────────────────────────────────────────

// Blinky — top-right corner scatter
std::pair<int, int> BlinkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                            Vec2) const {
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    int   tx   = static_cast<int>(std::floor((pac0Pos.x - offX) / tile));
    int   ty   = static_cast<int>(std::floor((pac0Pos.y - offY) / tile));
    return {tx, ty};
}
std::pair<int, int> BlinkyAI::getScatterTile(const LevelManager& level) const {
    return {level.getWidth() - 1, 0};
}

// Pinky — top-left corner scatter
std::pair<int, int> PinkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction facing, Vec2,
                                           Vec2) const {
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
std::pair<int, int> PinkyAI::getScatterTile(const LevelManager& level) const {
    return {0, 0};
}

// Inky — bottom-right corner scatter
std::pair<int, int> InkyAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2 pac1Pos,
                                          Vec2) const {
    float offX = (800.f - level.getWidth() * tile) / 2.f;
    float offY = (600.f - level.getHeight() * tile) / 2.f;
    float mx   = (pac0Pos.x + pac1Pos.x) * 0.5f;
    float my   = (pac0Pos.y + pac1Pos.y) * 0.5f;
    int   tx   = static_cast<int>(std::floor((mx - offX) / tile));
    int   ty   = static_cast<int>(std::floor((my - offY) / tile));
    return {std::clamp(tx, 0, level.getWidth() - 1), std::clamp(ty, 0, level.getHeight() - 1)};
}
std::pair<int, int> InkyAI::getScatterTile(const LevelManager& level) const {
    return {level.getWidth() - 1, level.getHeight() - 1};
}

// Clyde — bottom-left corner scatter
std::pair<int, int> ClydeAI::getTargetTile(const LevelManager& level, float tile, Vec2 pac0Pos, Direction, Vec2,
                                           Vec2 currentPos) const {
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
std::pair<int, int> ClydeAI::getScatterTile(const LevelManager& level) const {
    return {0, level.getHeight() - 1};
}
