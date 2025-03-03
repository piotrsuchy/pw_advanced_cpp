# Networked Pac-Man Game

PW - Piotr Suchy 310310
A C++ implementation of a multiplayer Pac-Man-like game using SFML for graphics and networking.

## Features

- Multiplayer gameplay over network
- Modern C++ design with OOP principles
- Graphical user interface using SFML
- Template-based grid system
- Network synchronization

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- SFML 2.5 or higher
- Git for version control

## Building the Project

1. Clone the repository:

    ```bash
    git clone [repository-url]
    cd pacman-game
    ```

2. Create a build directory:

    ```bash
    mkdir build
    cd build
    ```

3. Configure and build:

    ```bash
    cmake ..
    make
    ```

## Project Structure

- `src/core/` - Core game logic and entities
- `src/network/` - Networking components
- `src/utils/` - Utility classes and templates

## Class Overview

### Core Classes

- `Entity` - Base class for all game objects
- `Player` - Player implementation with movement and scoring
- `Ghost` - AI-controlled enemies
- `Collectible` - Power-ups and dots

### Network Classes

- `NetworkManager` - Handles client-server communication
- `GameSession` - Manages game state synchronization

### Utility Classes

- `Grid<T>` - Template class for managing the game maze
- `ResourceManager<T>` - Template class for resource handling
