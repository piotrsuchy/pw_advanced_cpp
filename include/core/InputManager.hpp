#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

enum class Direction { None, Up, Down, Left, Right };

class InputManager {
public: 
    void pollInputs(sf::RenderWindow& window);
    Direction getDirection() const;

private:
    Direction currentDirection = Direction::None;
};