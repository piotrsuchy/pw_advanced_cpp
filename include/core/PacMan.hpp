#pragma once
#include "core/Entity.hpp"
#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"

class PacMan : public Entity {
   public:
    PacMan();
    void         handleInput(Direction dir);
    void         update(float deltaTime, LevelManager& level, float scaledTileSize, float scale);
    void         draw(sf::RenderWindow& window) override;
    Direction    getDirection();
    sf::Vector2f getPosition();

   private:
    // sf::Vector2f position{400.f, 340.f};
    Direction direction{Direction::None};         // where it goes now
    Direction desiredDirection{Direction::None};  // queued turn
    Direction facingDirection{Direction::Right};  // last non-None facing
    float     speed{576.f};                       // 9 * 64.f

    bool aligned(float tile, float offX, float offY) const;
    bool canMove(Direction dir, const LevelManager& lvl, float tile, float offX, float offY);

    // Spritesheet texture (fallback) and optional separate textures
    sf::Texture texture;
    sf::Texture textureOpen;
    sf::Texture textureClosed;
    sf::Sprite  sprite;

    // Animation state
    sf::IntRect frameClosed{0, 0, 64, 64};
    sf::IntRect frameOpen{64, 0, 64, 64};
    bool        isMouthOpen{false};
    float       animationTimer{0.f};
    float       animationInterval{0.1f};  // seconds between open/close while moving

    bool useSeparateTextures{false};
    bool hasOpenTexture{false};
};