#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <tuple>
#include <vector>

#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"
#include "graphics/GhostRenderer.hpp"
#include "graphics/LevelRenderer.hpp"
#include "graphics/PacmanRenderer.hpp"
#include "shared/GameTypes.hpp"

// Names for the 4 ghost slots — purely for clarity at call sites
enum GhostSlot { Blinky = 0, Pinky = 1, Inky = 2, Clyde = 3 };

// Encapsulates the networked Pac-Man client:
//   - Connects to the server
//   - Sends player input
//   - Receives and parses SNAPSHOT / LEVEL packets
//   - Renders the game using SFML
class GameClient {
   public:
    explicit GameClient(sf::IpAddress serverIp, unsigned short port = 54000);

    // Blocking game loop — returns when the window is closed
    void run();

   private:
    // --- Setup ---
    void setupGhostTextures();
    void loadFont();

    // --- Per-frame operations ---
    void processWindowEvents();
    void sendInput();
    void receivePackets();
    void updateAnimations();
    void render();

    // --- Packet parsers ---
    void processSnapshot(sf::Packet& pkt);
    void processLevel(sf::Packet& pkt);

    // --- Rendering helpers ---
    void renderGhosts();
    void renderHUD();
    void renderPopups();

    // Network
    sf::TcpSocket  socket_;
    sf::IpAddress  serverIp_;
    unsigned short port_;

    // Window & rendering
    sf::RenderWindow window_;
    LevelManager     level_;
    LevelRenderer    levelRenderer_;
    PacmanRenderer   r0_, r1_;
    GhostRenderer    g0_, g1_, g2_, g3_;

    // Player state (received from server)
    float     p0x_{120.f}, p0y_{120.f};
    float     p1x_{680.f}, p1y_{480.f};
    uint16_t  score0_{0}, score1_{0};
    bool      pow0_{false}, pow1_{false};
    Direction f0_{Direction::Right}, f1_{Direction::Left};

    // Ghost state (received from server)
    std::array<float, 4>     gx_{400.f, 400.f, 400.f, 400.f};
    std::array<float, 4>     gy_{300.f, 300.f, 300.f, 300.f};
    std::array<Direction, 4> gf_{Direction::Left, Direction::Left, Direction::Left, Direction::Left};
    std::array<bool, 4>      ghostActive_{true, true, true, true};
    std::array<bool, 4>      ghostFrightened_{false, false, false, false};

    // Input
    InputManager input_;
    Direction    lastSent_{Direction::None};
    sf::Uint32   seq_{0};

    // HUD
    sf::Font hudFont_;
    bool     fontLoaded_{false};
    sf::Text scoreText0_, scoreText1_;

    // Animation state (estimated from positional deltas)
    float lp0x_{120.f}, lp0y_{120.f};
    float lp1x_{680.f}, lp1y_{480.f};

    // Ephemeral score popups: (x, y, points, age_seconds)
    std::vector<std::tuple<float, float, int, float>> popups_;
};
