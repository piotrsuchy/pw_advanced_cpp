#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "shared/GameTypes.hpp"

class InputManager {
   public:
    // call once for every SFML event
    void handleEvent(const sf::Event& e);

    // returns the next queued direction and clears the queue
    Direction popQueuedDirection();

   private:
    Direction queuedDir = Direction::None;  // one-shot buffer
};