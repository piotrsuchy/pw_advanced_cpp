#include "Player.h"
#include "Wall.h"
#include "Pellet.h"
#include <cmath>

Player::Player(float x, float y, const std::string &name)
    : Entity(x, y), name_(name), score_(0), isPoweredUp_(false), powerUpTimeLeft_(0.f), isCollidingWithWall_(false)
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
    // Check if we collided with a wall
    if (dynamic_cast<Wall *>(other))
    {
        // Calculate overlap and adjust position
        sf::Vector2f diff = position_ - other->getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // Collision radius (player radius + wall radius)
        const float COLLISION_RADIUS = 15.0f;

        if (distance < COLLISION_RADIUS)
        {
            // Only adjust position if we're actually overlapping
            if (distance > 0) // Prevent division by zero
            {
                // Normalize the difference vector
                diff.x /= distance;
                diff.y /= distance;

                // Move the player out of the wall by the overlap amount
                float overlap = COLLISION_RADIUS - distance;
                position_.x += diff.x * overlap;
                position_.y += diff.y * overlap;

                // Block movement only in the direction of the wall
                // If moving horizontally and hit a wall from the side
                if (std::abs(diff.x) > std::abs(diff.y))
                {
                    if ((velocity_.x > 0 && diff.x > 0) || (velocity_.x < 0 && diff.x < 0))
                    {
                        velocity_.x = 0;
                    }
                }
                // If moving vertically and hit a wall from top/bottom
                else
                {
                    if ((velocity_.y > 0 && diff.y > 0) || (velocity_.y < 0 && diff.y < 0))
                    {
                        velocity_.y = 0;
                    }
                }
            }
        }
    }
    else if (Pellet *pellet = dynamic_cast<Pellet *>(other))
    {
        if (pellet->isActive())
        {
            addScore(pellet->getPoints());
            if (pellet->isPowerPellet())
            {
                activatePowerUp(10.0f); // 10 seconds of power-up
            }
        }
    }
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