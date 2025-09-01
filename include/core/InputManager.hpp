#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

enum class Direction { None, Up, Down, Left, Right };

class InputManager {
   public:
    // call once for every SFML event
    void handleEvent(const sf::Event& e);

    // returns the next queued direction and clears the queue
    Direction popQueuedDirection();

   private:
    Direction queuedDir = Direction::None;  // one-shot buffer
};