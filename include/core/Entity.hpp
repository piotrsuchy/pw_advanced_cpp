#pragma once
#include <SFML/Graphics.hpp>
#include "core/InputManager.hpp"

// Forward declarations
class LevelManager;

class Entity
{
public:
    virtual ~Entity() = default;

    virtual void update(float deltaTime, LevelManager &level, float scaledTileSize, float scale) = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

    void setPosition(float x, float y)
    {
        position = sf::Vector2f(x, y);
        sprite.setPosition(position);
    }
    sf::Vector2f getPosition() const { return position; }

protected:
    sf::Sprite sprite;
    sf::Vector2f position;
    Direction direction = Direction::None;
    float speed = 64.f * 9;
};