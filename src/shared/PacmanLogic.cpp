#include "shared/PacmanLogic.hpp"

#include <cmath>

// ---------------------------------------------------------------------------
// Coordinate helpers
// ---------------------------------------------------------------------------

// Column/row of the tile that contains pixel coordinate px.
static int tileCol(float px, float offX, float tile) {
    return static_cast<int>(std::floor((px - offX) / tile));
}
static int tileRow(float py, float offY, float tile) {
    return static_cast<int>(std::floor((py - offY) / tile));
}

// Pixel coordinate of the *centre* of tile (col,row).
static float tileCentreX(int col, float offX, float tile) {
    return offX + col * tile + tile * 0.5f;
}
static float tileCentreY(int row, float offY, float tile) {
    return offY + row * tile + tile * 0.5f;
}

// ---------------------------------------------------------------------------
// PacmanLogic::update — tile-boundary collision
//
// Design contract (invariant maintained at all times):
//   Pac-Man's centre is always inside a non-wall tile.
//   Movement is clamped to the tile boundary before the position is committed,
//   so the centre can never penetrate a wall by even a single pixel.
// ---------------------------------------------------------------------------

void PacmanLogic::update(float dt, LevelManager& level, float scaledTileSize, float scale) {
    const float offX = (800.f - level.getWidth() * scaledTileSize) / 2.f;
    const float offY = (600.f - level.getHeight() * scaledTileSize) / 2.f;
    const float v    = speed * scale;

    // ── 1. Try to adopt the desired direction ─────────────────────────────
    // We allow a direction change whenever Pac-Man is aligned to the current
    // tile centre along the axis he is NOT moving on, and the target tile in
    // the desired direction is open.
    if (desiredDirection != Direction::None) {
        bool  tryTurn = false;
        int   col     = tileCol(position.x, offX, scaledTileSize);
        int   row     = tileRow(position.y, offY, scaledTileSize);
        float cx      = tileCentreX(col, offX, scaledTileSize);
        float cy      = tileCentreY(row, offY, scaledTileSize);

        bool desiredH = (desiredDirection == Direction::Left || desiredDirection == Direction::Right);
        bool desiredV = !desiredH;

        if (direction == Direction::None) {
            // Standing still — try any direction
            tryTurn = true;
        } else if (isPerpendicular(desiredDirection, direction)) {
            // Perpendicular turn: snap the off-axis position and check distance
            float offAxis = desiredH ? std::abs(position.y - cy) : std::abs(position.x - cx);
            tryTurn       = (offAxis <= CORNER_TOLERANCE);
        } else {
            // Same or opposite axis — can always queue (will commit when aligned)
            tryTurn = aligned(scaledTileSize, offX, offY);
        }

        if (tryTurn && canMove(desiredDirection, level, scaledTileSize, offX, offY)) {
            // Snap the perpendicular axis to the tile centre before turning.
            if (desiredH)
                position.y = cy;
            else
                position.x = cx;
            direction = desiredDirection;
        }
    }

    if (direction != Direction::None) facingDirection = direction;

    // ── 2. Stop if the tile ahead is a wall ───────────────────────────────
    // Check this *before* moving so we can clamp to the exact tile boundary.
    if (direction != Direction::None) {
        int col = tileCol(position.x, offX, scaledTileSize);
        int row = tileRow(position.y, offY, scaledTileSize);

        int nextCol = col, nextRow = row;
        switch (direction) {
            case Direction::Left:
                --nextCol;
                break;
            case Direction::Right:
                ++nextCol;
                break;
            case Direction::Up:
                --nextRow;
                break;
            case Direction::Down:
                ++nextRow;
                break;
            default:
                break;
        }

        bool wallAhead =
            (level.getTile(nextCol, nextRow) == TileType::Wall) || (nextRow < 0 || nextRow >= level.getHeight());

        if (wallAhead) {
            // Clamp Pac-Man to the exact centre of the current tile so he
            // never pokes into the wall tile, even by a sub-pixel amount.
            float cx = tileCentreX(col, offX, scaledTileSize);
            float cy = tileCentreY(row, offY, scaledTileSize);

            // Only stop when we have reached (or passed) the tile centre —
            // i.e. we are actually approaching the wall, not moving away.
            bool approaching = false;
            switch (direction) {
                case Direction::Left:
                    approaching = (position.x <= cx + 1.f);
                    break;
                case Direction::Right:
                    approaching = (position.x >= cx - 1.f);
                    break;
                case Direction::Up:
                    approaching = (position.y <= cy + 1.f);
                    break;
                case Direction::Down:
                    approaching = (position.y >= cy - 1.f);
                    break;
                default:
                    break;
            }

            if (approaching) {
                // Snap to tile centre and stop.
                position.x = cx;
                position.y = cy;
                direction  = Direction::None;
                // Keep desiredDirection so the player's input is buffered.
                return;
            }
        }
    }

    // ── 3. Move ───────────────────────────────────────────────────────────
    if (direction != Direction::None) {
        Vec2 next = position;
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

        // ── 4. Leading-edge wall check ────────────────────────────────────
        // We check the tile the *leading edge* of Pac-Man will enter, not
        // just the tile his centre is in.  The leading edge is half a tile
        // away from the centre in the direction of travel.
        const float halfTile = scaledTileSize * 0.5f;
        float       edgeX    = next.x;
        float       edgeY    = next.y;
        switch (direction) {
            case Direction::Left:
                edgeX -= halfTile;
                break;
            case Direction::Right:
                edgeX += halfTile;
                break;
            case Direction::Up:
                edgeY -= halfTile;
                break;
            case Direction::Down:
                edgeY += halfTile;
                break;
            default:
                break;
        }

        int edgeCol = tileCol(edgeX, offX, scaledTileSize);
        int edgeRow = tileRow(edgeY, offY, scaledTileSize);

        if (edgeRow < 0 || edgeRow >= level.getHeight() || level.getTile(edgeCol, edgeRow) == TileType::Wall) {
            // Clamp: move Pac-Man so his leading edge sits exactly at the
            // wall boundary (== tile centre of current tile).
            int   col  = tileCol(position.x, offX, scaledTileSize);
            int   row  = tileRow(position.y, offY, scaledTileSize);
            float cx   = tileCentreX(col, offX, scaledTileSize);
            float cy   = tileCentreY(row, offY, scaledTileSize);
            position.x = cx;
            position.y = cy;
            direction  = Direction::None;
            // desiredDirection is kept so input is buffered.
        } else {
            position = next;

            // ── 5. Keep Pac-Man centred on the perpendicular axis ─────────
            // This prevents gradual drift from sub-pixel rounding errors and
            // ensures ghost collision always sees the canonical position.
            int   col = tileCol(position.x, offX, scaledTileSize);
            int   row = tileRow(position.y, offY, scaledTileSize);
            float cx  = tileCentreX(col, offX, scaledTileSize);
            float cy  = tileCentreY(row, offY, scaledTileSize);
            if (direction == Direction::Left || direction == Direction::Right)
                position.y = cy;
            else if (direction == Direction::Up || direction == Direction::Down)
                position.x = cx;

            // ── 6. Tunnel wrap ────────────────────────────────────────────
            const float worldLeft  = offX;
            const float worldRight = offX + level.getWidth() * scaledTileSize;
            if (position.x < worldLeft) position.x += level.getWidth() * scaledTileSize;
            if (position.x >= worldRight) position.x -= level.getWidth() * scaledTileSize;
        }
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

bool PacmanLogic::aligned(float tile, float offX, float offY) const {
    constexpr float eps = 4.f;
    int             col = tileCol(position.x, offX, tile);
    int             row = tileRow(position.y, offY, tile);
    float           cx  = tileCentreX(col, offX, tile);
    float           cy  = tileCentreY(row, offY, tile);
    return (std::abs(position.x - cx) < eps) && (std::abs(position.y - cy) < eps);
}

bool PacmanLogic::canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY) {
    if (dir == Direction::None) return false;
    int col = tileCol(position.x, offX, tile);
    int row = tileRow(position.y, offY, tile);
    switch (dir) {
        case Direction::Up:
            --row;
            break;
        case Direction::Down:
            ++row;
            break;
        case Direction::Left:
            --col;
            break;
        case Direction::Right:
            ++col;
            break;
        default:
            break;
    }
    if (row < 0 || row >= lvl.getHeight()) return false;
    return lvl.getTile(col, row) != TileType::Wall;
}

bool PacmanLogic::isPerpendicular(Direction a, Direction b) const {
    if (a == Direction::None || b == Direction::None) return false;
    bool aH = (a == Direction::Left || a == Direction::Right);
    bool bH = (b == Direction::Left || b == Direction::Right);
    return aH != bH;
}

bool PacmanLogic::canCornerTurn(Direction newDir, float tile, float offX, float offY, const LevelManager& lvl) const {
    if (!isPerpendicular(newDir, direction)) return false;
    if (newDir == Direction::None) return false;

    int   col = tileCol(position.x, offX, tile);
    int   row = tileRow(position.y, offY, tile);
    float cx  = tileCentreX(col, offX, tile);
    float cy  = tileCentreY(row, offY, tile);

    float distToCenter = 0.f;
    if (direction == Direction::Left || direction == Direction::Right)
        distToCenter = std::abs(position.x - cx);
    else if (direction == Direction::Up || direction == Direction::Down)
        distToCenter = std::abs(position.y - cy);

    if (distToCenter > CORNER_TOLERANCE) return false;

    int targetCol = col, targetRow = row;
    switch (newDir) {
        case Direction::Up:
            --targetRow;
            break;
        case Direction::Down:
            ++targetRow;
            break;
        case Direction::Left:
            --targetCol;
            break;
        case Direction::Right:
            ++targetCol;
            break;
        default:
            return false;
    }
    if (targetCol < 0 || targetRow < 0 || targetCol >= lvl.getWidth() || targetRow >= lvl.getHeight()) return false;
    return lvl.getTile(targetCol, targetRow) != TileType::Wall;
}
