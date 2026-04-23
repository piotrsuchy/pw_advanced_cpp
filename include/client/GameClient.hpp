#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <string>
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

enum class ClientFlow { MainMenu, InGame };
enum class SubMenu { None, Options };

// Encapsulates the networked Pac-Man client:
//   - Main menu → connect → READY → play (pause / end screens)
//   - Sends input, PAUSE, RESTART; applies SNAPSHOT / LEVEL
//   - Renders with SFML
class GameClient {
   public:
    GameClient(sf::IpAddress serverIp, unsigned short port, int localPlayerIndex);

    void run();

   private:
    // --- Setup ---
    void setupGhostTextures();
    void loadFont();

    // --- Per-frame operations ---
    void processWindowEvents();
    void sendInput();
    void sendSimplePacket(const char* kind);
    void receivePackets();
    void updateAnimations(float dt);
    void render();
    void disconnectToMenu();
    bool connectToServer();
    bool canPlayerSteer() const;

    // --- UI ---
    void handleMainMenuKey(sf::Keyboard::Key k);
    void drawMainMenu();
    void drawOptionsOverlay();
    void renderStatusLine(const char* line);
    void drawEndScreen(bool victory);
    void drawPauseOverlay();
    void drawReadyOverlay();
    void renderDimBackground(float alpha);

    // --- Packet parsers ---
    void processSnapshot(sf::Packet& pkt);
    void processLevel(sf::Packet& pkt);

    // --- Rendering helpers ---
    void renderHUD();
    void renderPopups();
    void drawCenteredText(const sf::Text& t, float y);

    // Network
    sf::TcpSocket  socket_;
    sf::IpAddress  serverIp_;
    unsigned short port_;
    int            localPlayerIndex_{0};

    // UI flow
    ClientFlow  flow_{ClientFlow::MainMenu};
    SubMenu     subMenu_{SubMenu::None};
    int         mainMenuIndex_{0};
    float       readyTimer_{0.f};
    bool        haveLevel_{false};
    std::string menuError_;
    float       menuErrorTime_{0.f};

    bool      serverPaused_{false};
    sf::Uint8 matchOutcome_{0};

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
    unsigned  lives0_{3}, lives1_{3};
    bool      pow0_{false}, pow1_{false};
    float     powerTime0_{0.f}, powerTime1_{0.f};
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
