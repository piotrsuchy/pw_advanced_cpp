#include "Game.h"

Game::Game(sf::RenderWindow &window)
    : window_(window), maze_(28, 31, 0) // Standard Pac-Man maze size
{
    // Initialize shapes
    playerShape_.setRadius(10.f);
    playerShape_.setFillColor(sf::Color::Yellow);
    playerShape_.setOrigin(10.f, 10.f);

    wallShape_.setSize(sf::Vector2f(20.f, 20.f));
    wallShape_.setFillColor(sf::Color::Blue);
    wallShape_.setOrigin(10.f, 10.f);

    pelletShape_.setRadius(3.f);
    pelletShape_.setFillColor(sf::Color::White);
    pelletShape_.setOrigin(3.f, 3.f);

    powerPelletShape_.setRadius(8.f);
    powerPelletShape_.setFillColor(sf::Color::White);
    powerPelletShape_.setOrigin(8.f, 8.f);
}

void Game::initialize()
{
    loadMaze();

    // Add a player
    auto player = std::make_unique<Player>(400.f, 300.f, "Player1");
    players_.push_back(player.get());
    entities_.push_back(std::move(player));
}

void Game::update(float deltaTime)
{
    // Update all entities
    for (auto &entity : entities_)
    {
        if (entity->getPosition().x < 0)
            entity->setPosition(window_.getSize().x, entity->getPosition().y);
        if (entity->getPosition().x > window_.getSize().x)
            entity->setPosition(0, entity->getPosition().y);
        if (entity->getPosition().y < 0)
            entity->setPosition(entity->getPosition().x, window_.getSize().y);
        if (entity->getPosition().y > window_.getSize().y)
            entity->setPosition(entity->getPosition().x, 0);

        entity->update(deltaTime);
    }

    checkCollisions();
}

void Game::render()
{
    window_.clear(sf::Color::Black);

    // Render all entities
    for (const auto &entity : entities_)
    {
        if (auto player = dynamic_cast<Player *>(entity.get()))
        {
            playerShape_.setPosition(player->getPosition());
            window_.draw(playerShape_);
        }
        else if (auto wall = dynamic_cast<Wall *>(entity.get()))
        {
            wallShape_.setPosition(wall->getPosition());
            window_.draw(wallShape_);
        }
        else if (auto pellet = dynamic_cast<Pellet *>(entity.get()))
        {
            if (pellet->isPowerPellet())
            {
                powerPelletShape_.setPosition(pellet->getPosition());
                window_.draw(powerPelletShape_);
            }
            else
            {
                pelletShape_.setPosition(pellet->getPosition());
                window_.draw(pelletShape_);
            }
        }
    }

    window_.display();
}

void Game::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        for (auto player : players_)
        {
            float speed = 200.f;
            switch (event.key.code)
            {
            case sf::Keyboard::Left:
                player->setVelocity(-speed, 0.f);
                break;
            case sf::Keyboard::Right:
                player->setVelocity(speed, 0.f);
                break;
            case sf::Keyboard::Up:
                player->setVelocity(0.f, -speed);
                break;
            case sf::Keyboard::Down:
                player->setVelocity(0.f, speed);
                break;
            default:
                break;
            }
        }
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        for (auto player : players_)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
                player->setVelocity(0.f, 0.f);
                break;
            default:
                break;
            }
        }
    }
}

void Game::loadMaze()
{
    // a simple maze for testing
    // 0 = empty, 1 = wall, 2 = pellet, 3 = power pellet
    const int testMaze[31][28] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
        {1, 3, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 3, 1},
        {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1},
        {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1},
        {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1},
        {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
        {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };

    // Copy maze data and create entities
    for (int y = 0; y < 31; ++y)
    {
        for (int x = 0; x < 28; ++x)
        {
            maze_.at(x, y) = testMaze[y][x];
            float posX = x * 20.f + 10.f;
            float posY = y * 20.f + 10.f;

            switch (testMaze[y][x])
            {
            case 1: // Wall
                entities_.push_back(std::make_unique<Wall>(posX, posY));
                break;
            case 2: // Regular pellet
                entities_.push_back(std::make_unique<Pellet>(posX, posY, false));
                break;
            case 3: // Power pellet
                entities_.push_back(std::make_unique<Pellet>(posX, posY, true));
                break;
            }
        }
    }
}

void Game::checkCollisions()
{
    for (size_t i = 0; i < entities_.size(); ++i)
    {
        for (size_t j = i + 1; j < entities_.size(); ++j)
        {
            auto &entity1 = entities_[i];
            auto &entity2 = entities_[j];

            // Simple circle collision detection
            sf::Vector2f diff = entity1->getPosition() - entity2->getPosition();
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (distance < 20.f) // Collision radius
            {
                entity1->onCollision(entity2.get());
                entity2->onCollision(entity1.get());
            }
        }
    }

    // Remove inactive entities (collected pellets)
    entities_.erase(
        std::remove_if(entities_.begin(), entities_.end(),
                       [](const std::unique_ptr<Entity> &entity)
                       {
                           return !entity->isActive();
                       }),
        entities_.end());
}