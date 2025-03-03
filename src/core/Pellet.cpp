#include "Pellet.h"
#include "Player.h"

Pellet::Pellet(float x, float y, bool isPowerPellet)
    : Entity(x, y), isPowerPellet_(isPowerPellet)
{
}

void Pellet::update(float deltaTime)
{
    // Pellets are static, no update needed
}

void Pellet::onCollision(Entity *other)
{
    if (!active_)
        return;

    Player *player = dynamic_cast<Player *>(other);
    if (player)
    {
        player->addScore(getPoints());
        if (isPowerPellet_)
        {
            player->activatePowerUp(10.0f); // 10 seconds of power-up
        }
        active_ = false; // Pellet is collected
    }
}