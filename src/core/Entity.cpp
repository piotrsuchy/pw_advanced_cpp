#include "Entity.h"

Entity::Entity(float x, float y)
    : position_(x, y), velocity_(0.f, 0.f), active_(true)
{
}

void Entity::setPosition(float x, float y)
{
    position_.x = x;
    position_.y = y;
}

void Entity::setVelocity(float vx, float vy)
{
    velocity_.x = vx;
    velocity_.y = vy;
}