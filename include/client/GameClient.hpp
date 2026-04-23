#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "client/AudioManager.hpp"
#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"
#include "graphics/GhostRenderer.hpp"
#include "graphics/LevelRenderer.hpp"
#include "graphics/PacmanRenderer.hpp"
#include "shared/GameTypes.hpp"

/**
 * @brief Named indices for the four ghost slots sent by the server.
 */
enum GhostSlot {
    Blinky = 0,  ///< Red ghost that directly chases player 0.
    Pinky  = 1,  ///< Pink ghost that targets tiles ahead of player 0.
    Inky   = 2,  ///< Cyan ghost whose chase target depends on both players.
    Clyde  = 3,  ///< Orange ghost that alternates between chase and retreat.
};

/**
 * @brief Top-level flow states for the client application.
 */
enum class ClientFlow {
    MainMenu,  ///< Client is showing menus and is not actively playing.
    InGame,    ///< Client is connected to a running match view.
};

/**
 * @brief Submenu states shown from the main menu.
 */
enum class SubMenu {
    None,     ///< No submenu overlay is currently visible.
    Options,  ///< The options overlay is currently open.
};

/**
 * @brief Networked SFML client for the multiplayer Pac-Man game.
 *
 * The client owns the application window, menus, renderers, audio playback,
 * and the TCP connection used to mirror authoritative server state.
 */
class GameClient {
   public:
    /**
     * @brief Creates a client configured for one player slot and server target.
     *
     * @param serverIp Server IP address to connect to.
     * @param port TCP port exposed by the game server.
     * @param localPlayerIndex Local player slot index used by the server.
     */
    GameClient(sf::IpAddress serverIp, unsigned short port, int localPlayerIndex);

    /**
     * @brief Runs the main client loop until the window closes.
     */
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
    bool        playerReadySent_{false};
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
    float    p0x_{120.f}, p0y_{120.f};
    float    p1x_{680.f}, p1y_{480.f};
    uint16_t score0_{0}, score1_{0};
    unsigned lives0_{3}, lives1_{3};
    bool     pow0_{false}, pow1_{false};
    float    powerTime0_{0.f}, powerTime1_{0.f};
    /**
     * @brief Shared phase for the power-pellet end flash animation.
     *
     * A single clock keeps all ghost flash states in sync and avoids resets from
     * per-ghost frightened toggles or one-frame snapshot glitches.
     */
    float     powerFrightenPhase_{0.f};
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

    // Audio (snapshot edges + bundled assets)
    AudioManager audio_;

    // Ephemeral score popups: (x, y, points, age_seconds)
    std::vector<std::tuple<float, float, int, float>> popups_;
};
