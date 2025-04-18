#pragma once
#include "core/Entity.hpp"
#include "core/LevelManager.hpp"
#include "core/InputManager.hpp"

class PacMan : public Entity
{
public:
    PacMan();
    void handleInput(Direction dir);
    void update(float deltaTime, LevelManager &level, float scaledTileSize, float scale);
    void draw(sf::RenderWindow &window) override;

private:
    sf::Texture texture;
};