#include "Wall.h"

Wall::Wall(float x, float y)
    : Entity(x, y)
{
}

void Wall::update(float deltaTime)
{
    // Walls are static, no update needed
}

void Wall::onCollision(Entity *other)
{
    // Walls block movement, handled by the colliding entity
}