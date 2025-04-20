#include "core/PacMan.hpp"
#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp" // For tileSize

PacMan::PacMan()
{
    if (!texture.loadFromFile("assets/textures/pacman.png"))
    {
        sprite.setTextureRect(sf::IntRect(0, 0, 64, 64)); // fallback rectangle
    }
    else
    {
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    }

    sprite.setColor(sf::Color::Yellow);
    sprite.setOrigin(32.f, 32.f); // Center of 64x64 sprite

    // Scale sprite to match tile size (64x64)
    sprite.setScale(1.0f, 1.0f); // Will be adjusted in draw method

    // Start position will be set by GameController
    position = sf::Vector2f(400.f, 300.f);
    sprite.setPosition(position);
}

void PacMan::handleInput(Direction dir)
{
    direction = dir;
}

void PacMan::update(float deltaTime, LevelManager &level, float scaledTileSize, float scale)
{
    sf::Vector2f movement(0.f, 0.f);
    sf::Vector2f nextPosition = position;

    // Apply scaling to the sprite
    sprite.setScale(scale, scale);

    // Adjust speed based on scale
    float adjustedSpeed = speed * scale;

    switch (direction)
    {
    case Direction::Up:
        nextPosition.y -= adjustedSpeed * deltaTime;
        break;
    case Direction::Down:
        nextPosition.y += adjustedSpeed * deltaTime;
        break;
    case Direction::Left:
        nextPosition.x -= adjustedSpeed * deltaTime;
        break;
    case Direction::Right:
        nextPosition.x += adjustedSpeed * deltaTime;
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

void PacMan::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}