#pragma once
#include "core/Entity.hpp"
#include "core/InputManager.hpp"

class PacMan : public Entity {
public:
    PacMan();
    void handleInput(Direction dir);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Texture texture;
};