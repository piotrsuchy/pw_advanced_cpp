#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "shared/GameTypes.hpp"

/**
 * @brief Buffers the latest movement input coming from SFML keyboard events.
 *
 * The client uses this helper to convert raw window events into a persistent
 * desired direction that can be sent to the server or consumed by local logic.
 */
class InputManager {
   public:
    /**
     * @brief Consumes one SFML event and updates the queued direction if needed.
     *
     * @param e Event received from the render window.
     */
    void handleEvent(const sf::Event& e);

    /**
     * @brief Returns the currently queued direction.
     *
     * The direction remains queued until another key press overrides it or
     * `clearQueuedDirection()` is called.
     *
     * @return The buffered desired direction.
     */
    Direction popQueuedDirection();

    /**
     * @brief Returns the queued direction without clearing or modifying it.
     *
     * @return The buffered desired direction.
     */
    Direction getDesiredDirection() const;

    /**
     * @brief Clears the queued direction after it has been applied.
     */
    void clearQueuedDirection();

   private:
    Direction queuedDir = Direction::None;  // persistent buffer until new key
};