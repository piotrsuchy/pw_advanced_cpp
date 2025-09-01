#include <SFML/System.hpp>  // for Vector2f
#include <ostream>

#include "core/InputManager.hpp"

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

std::ostream& operator<<(std::ostream& os, Direction d) {
    switch (d) {
        case Direction::None:
            return os << "None";
        case Direction::Up:
            return os << "Up";
        case Direction::Down:
            return os << "Down";
        case Direction::Left:
            return os << "Left";
        case Direction::Right:
            return os << "Right";
    }
    return os;  // silence warning: not all control paths ...
}