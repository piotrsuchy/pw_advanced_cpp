#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "shared/GameTypes.hpp"

class InputManager {
   public:
    // call once for every SFML event
    void handleEvent(const sf::Event& e);

    // returns the queued direction (persists until a new key is pressed)
    Direction popQueuedDirection();

    // returns the desired direction without modifying it (non-destructive read)
    Direction getDesiredDirection() const;

    // clears the queued direction (call when turn is successfully applied)
    void clearQueuedDirection();

   private:
    Direction queuedDir = Direction::None;  // persistent buffer until new key
};