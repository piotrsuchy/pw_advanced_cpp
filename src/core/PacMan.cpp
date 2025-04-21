#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp" // For tileSize

void PacMan::handleInput(Direction dir)
{
    direction = dir;
}

void PacMan::update(float dt, LevelManager &level, float scaledTileSize, float scale)
{
    sf::Vector2f nextPosition = position;

    // Apply scaling to the sprite
    sprite.setScale(scale, scale);

    // Adjust speed based on scale
    float adjustedSpeed = speed * scale;

    switch (direction)
    {
    case Direction::Up:
        nextPosition.y -= adjustedSpeed * dt;
        break;
    case Direction::Down:
        nextPosition.y += adjustedSpeed * dt;
        break;
    case Direction::Left:
        nextPosition.x -= adjustedSpeed * dt;
        break;
    case Direction::Right:
        nextPosition.x += adjustedSpeed * dt;
        break;
    default:
        break;
    }

    // Convert screen coordinates to grid coordinates
    float offsetX = (800.f - level.getWidth() * scaledTileSize) / 2.0f;
    float offsetY = (600.f - level.getHeight() * scaledTileSize) / 2.0f;

    int tileX = static_cast<int>((nextPosition.x - offsetX) / scaledTileSize);
    int tileY = static_cast<int>((nextPosition.y - offsetY) / scaledTileSize);

    if (tileX >= 0 && tileY >= 0 && tileX < level.getWidth() && tileY < level.getHeight())
    {
        if (level.getTile(tileX, tileY) != TileType::Wall)
        {
            position = nextPosition;
            sprite.setPosition(position);
            level.collectPellet(tileX, tileY);
        }
    }
}