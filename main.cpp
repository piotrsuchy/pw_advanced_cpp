// pacman.cpp
#include <SFML/Graphics.hpp>
#include <vector>

const int TILE_SIZE = 32;
const int WIDTH = 20;
const int HEIGHT = 15;

enum Tile {
    WALL,
    EMPTY
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void loadLevel();

    sf::RenderWindow window;
    sf::RectangleShape tileShape;
    sf::CircleShape pacman;
    std::vector<std::vector<Tile>> map;
    sf::Vector2i pacmanPos;
    sf::Vector2i direction;
};

Game::Game() : window(sf::VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "PacMan"), pacmanPos(1, 1), direction(0, 0) {
    tileShape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    pacman.setRadius(TILE_SIZE / 2 - 2);
    pacman.setFillColor(sf::Color::Yellow);
    loadLevel();
}

void Game::loadLevel() {
    map = std::vector<std::vector<Tile>>(HEIGHT, std::vector<Tile>(WIDTH, EMPTY));
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1 || (x % 4 == 0 && y % 4 == 0)) {
                map[y][x] = WALL;
            }
        }
    }
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up: direction = {0, -1}; break;
                case sf::Keyboard::Down: direction = {0, 1}; break;
                case sf::Keyboard::Left: direction = {-1, 0}; break;
                case sf::Keyboard::Right: direction = {1, 0}; break;
                default: break;
            }
        }
    }
}

void Game::update() {
    sf::Vector2i newPos = pacmanPos + direction;
    if (map[newPos.y][newPos.x] == EMPTY) {
        pacmanPos = newPos;
    }
}

void Game::render() {
    window.clear();

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            tileShape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            if (map[y][x] == WALL) {
                tileShape.setFillColor(sf::Color::Blue);
                window.draw(tileShape);
            }
        }
    }

    pacman.setPosition(pacmanPos.x * TILE_SIZE + 2, pacmanPos.y * TILE_SIZE + 2);
    window.draw(pacman);

    window.display();
}

int main() {
    Game game;
    game.run();
    return 0;
}
