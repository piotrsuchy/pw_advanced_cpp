#include "core/Entity.hpp"

void Entity::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
}

sf::Vector2f Entity::getPosition() const {
    return position;
}