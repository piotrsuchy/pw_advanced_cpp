#include "Player.h"

Player::Player(float x, float y, const std::string &name)
    : Entity(x, y), name_(name), score_(0), isPoweredUp_(false), powerUpTimeLeft_(0.f)
{
}

void Player::update(float deltaTime)
{
    // Update position based on velocity
    position_.x += velocity_.x * deltaTime;
    position_.y += velocity_.y * deltaTime;

    // Update power-up state
    if (isPoweredUp_)
    {
        powerUpTimeLeft_ -= deltaTime;
        if (powerUpTimeLeft_ <= 0.f)
        {
            isPoweredUp_ = false;
            powerUpTimeLeft_ = 0.f;
        }
    }
}

void Player::onCollision(Entity *other)
{
    // Collision handling will be implemented based on the type of entity
    // This will use dynamic_cast to determine the type of entity and respond accordingly
}

void Player::addScore(int points)
{
    score_ += points;
}

void Player::activatePowerUp(float duration)
{
    isPoweredUp_ = true;
    powerUpTimeLeft_ = duration;
}