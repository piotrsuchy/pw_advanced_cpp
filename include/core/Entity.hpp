#pragma once
#include <SFML/Graphics.hpp>
#include "core/InputManager.hpp"

class Entity {
public:
    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

protected:
    sf::Sprite sprite;
    sf::Vector2f position;
    Direction direction = Direction::None;
    float speed = 100.f;
};