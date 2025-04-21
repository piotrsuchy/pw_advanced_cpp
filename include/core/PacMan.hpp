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
    Direction getDirection();
    sf::Vector2f getPosition();

private:
    // sf::Vector2f position{400.f, 340.f};
    Direction direction{Direction::None}; // where it goes now
    Direction desiredDirection{Direction::None}; // queued turn
    float speed{576.f}; // 9 * 64.f

    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl,
                 float tile, float offX, float offY);

    sf::Texture texture;
    sf::Sprite sprite;
};