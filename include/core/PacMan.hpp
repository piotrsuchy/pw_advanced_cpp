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
    sf::Vector2f position{400.f, 350.f};
    Direction direction{Direction::None};
    float speed{576.f}; // 9 * 64.f

    sf::Texture texture;
    sf::Sprite sprite;
};