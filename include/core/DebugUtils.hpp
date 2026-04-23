#include <SFML/System.hpp>  // for Vector2f
#include <ostream>

#include "core/InputManager.hpp"

/**
 * @brief Streams an `sf::Vector2f` in `(x, y)` form for debugging output.
 *
 * @param os Output stream to write to.
 * @param v Vector value to print.
 * @return The same output stream for chaining.
 */
std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

/**
 * @brief Streams a `Direction` enum as a readable string.
 *
 * @param os Output stream to write to.
 * @param d Direction value to print.
 * @return The same output stream for chaining.
 */
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