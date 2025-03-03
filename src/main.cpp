#include <SFML/Graphics.hpp>
#include "core/Game.h"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(580, 660), "Pac-Man Game");
    window.setFramerateLimit(60);

    // Create and initialize the game
    Game game(window);
    game.initialize();

    sf::Clock clock;

    // Game loop
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                game.handleEvent(event);
        }

        // Update game state
        float deltaTime = clock.restart().asSeconds();
        game.update(deltaTime);

        // Render
        game.render();
    }

    return 0;
}