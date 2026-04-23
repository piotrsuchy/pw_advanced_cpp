#include "client/GameClient.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Construction / setup
// ---------------------------------------------------------------------------

GameClient::GameClient(sf::IpAddress serverIp, unsigned short port)
    : serverIp_(serverIp), port_(port), window_(sf::VideoMode(800, 600), "Pacman Client") {
    window_.setFramerateLimit(60);
    level_.loadLevel(1);
    setupGhostTextures();
    loadFont();
}

void GameClient::setupGhostTextures() {
    g0_.setDirectionalTextures("assets/textures/ghosts/blinky_up.png", "assets/textures/ghosts/blinky_down.png",
                               "assets/textures/ghosts/blinky_left.png", "assets/textures/ghosts/blinky_right.png");
    g1_.setDirectionalTextures("assets/textures/ghosts/pinky_up.png", "assets/textures/ghosts/pinky_down.png",
                               "assets/textures/ghosts/pinky_left.png", "assets/textures/ghosts/pinky_right.png");
    g2_.setDirectionalTextures("assets/textures/ghosts/inky_up.png", "assets/textures/ghosts/inky_down.png",
                               "assets/textures/ghosts/inky_left.png", "assets/textures/ghosts/inky_right.png");
    g3_.setDirectionalTextures("assets/textures/ghosts/clyde_up.png", "assets/textures/ghosts/clyde_down.png",
                               "assets/textures/ghosts/clyde_left.png", "assets/textures/ghosts/clyde_right.png");
}

void GameClient::loadFont() {
    // 1. Env-var override
    if (const char* envFont = std::getenv("PACMAN_FONT")) fontLoaded_ = hudFont_.loadFromFile(envFont);

    // 2. Bundled assets
    if (!fontLoaded_) {
        const char* assetFonts[] = {
            "assets/fonts/DejaVuSans.ttf",
            "assets/fonts/FreeSans.ttf",
            "assets/fonts/OpenSans-Regular.ttf",
            "assets/fonts/PressStart2P.ttf",
        };
        for (const char* p : assetFonts)
            if (hudFont_.loadFromFile(p)) {
                fontLoaded_ = true;
                break;
            }
    }

    // 3. System fallbacks
    if (!fontLoaded_) {
        const char* systemFonts[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Helvetica.ttf",
        };
        for (const char* p : systemFonts)
            if (hudFont_.loadFromFile(p)) {
                fontLoaded_ = true;
                break;
            }
    }

    if (!fontLoaded_) {
        std::cerr << "[CLIENT] Warning: no font loaded (set PACMAN_FONT or add assets/fonts/*.ttf)\n";
        return;
    }

    auto setup = [&](sf::Text& t, sf::Color col) {
        t.setFont(hudFont_);
        t.setCharacterSize(24);
        t.setFillColor(col);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(2.f);
    };
    setup(scoreText0_, sf::Color(255, 255, 0));  // P1 — yellow
    setup(scoreText1_, sf::Color(0, 200, 255));  // P2 — cyan
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void GameClient::run() {
    if (socket_.connect(serverIp_, port_) != sf::Socket::Done) {
        std::cerr << "[CLIENT] connect failed\n";
        return;
    }
    socket_.setBlocking(false);

    while (window_.isOpen()) {
        processWindowEvents();
        sendInput();
        receivePackets();
        updateAnimations();
        render();
    }
}

// ---------------------------------------------------------------------------
// Per-frame operations
// ---------------------------------------------------------------------------

void GameClient::processWindowEvents() {
    sf::Event e;
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window_.close();
        input_.handleEvent(e);
    }
}

void GameClient::sendInput() {
    Direction d = input_.popQueuedDirection();
    if (d == Direction::None || d == lastSent_) return;
    lastSent_ = d;
    sf::Packet pkt;
    pkt << std::string("INPUT") << seq_++ << static_cast<sf::Uint8>(d);
    socket_.send(pkt);
}

void GameClient::receivePackets() {
    while (true) {
        sf::Packet pkt;
        if (socket_.receive(pkt) != sf::Socket::Done) break;
        std::string kind;
        pkt >> kind;
        if (kind == "SNAPSHOT")
            processSnapshot(pkt);
        else if (kind == "LEVEL")
            processLevel(pkt);
    }
}

void GameClient::processSnapshot(sf::Packet& pkt) {
    sf::Uint16 s0, s1;
    sf::Uint8  pw0, pw1, l0, l1;
    float      dt0, dt1;
    pkt >> p0x_ >> p0y_ >> s0 >> pw0 >> l0 >> dt0;
    pkt >> p1x_ >> p1y_ >> s1 >> pw1 >> l1 >> dt1;

    score0_ = s0;
    score1_ = s1;
    pow0_   = (pw0 != 0);
    pow1_   = (pw1 != 0);

    r0_.setDeathTimeLeft(dt0);
    r1_.setDeathTimeLeft(dt1);

    // 4 ghost positions
    for (int gi = 0; gi < 4; ++gi) pkt >> gx_[gi] >> gy_[gi];

    // 4 ghost facings
    for (int gi = 0; gi < 4; ++gi) {
        sf::Uint8 f;
        pkt >> f;
        gf_[gi] = static_cast<Direction>(f);
    }

    // Consumed pellets
    sf::Uint16 nPellets, nGhostScores;
    pkt >> nPellets >> nGhostScores;

    // Per-ghost active + frightened flags
    for (int gi = 0; gi < 4; ++gi) {
        sf::Uint8 a, fr;
        pkt >> a >> fr;
        ghostActive_[gi]     = (a != 0);
        ghostFrightened_[gi] = (fr != 0);
    }

    // Pellet deltas — clear consumed tiles on the client's copy of the level
    for (sf::Uint16 k = 0; k < nPellets; ++k) {
        sf::Uint16 cx, cy;
        sf::Uint8  t;
        pkt >> cx >> cy >> t;
        if (t == 1 || t == 2) level_.setTile(cx, cy, TileType::Empty);
    }

    // Ghost-eat score popups
    for (sf::Uint16 k = 0; k < nGhostScores; ++k) {
        float      ex, ey;
        sf::Uint16 pts;
        pkt >> ex >> ey >> pts;
        popups_.emplace_back(ex, ey, static_cast<int>(pts), 0.f);
    }
}

void GameClient::processLevel(sf::Packet& pkt) {
    sf::Uint16 w, h;
    pkt >> w >> h;
    for (int y = 0; y < static_cast<int>(h); ++y) {
        for (int x = 0; x < static_cast<int>(w); ++x) {
            sf::Uint8 tv;
            pkt >> tv;
            TileType t = TileType::Empty;
            if (tv == 1)
                t = TileType::Wall;
            else if (tv == 2)
                t = TileType::Pellet;
            else if (tv == 3)
                t = TileType::PowerPellet;
            else if (tv == 4)
                t = TileType::Cherry;
            level_.setTile(x, y, t);
        }
    }
}

void GameClient::updateAnimations() {
    // Infer facing direction from positional deltas
    float dv0x = p0x_ - lp0x_, dv0y = p0y_ - lp0y_;
    float dv1x = p1x_ - lp1x_, dv1y = p1y_ - lp1y_;

    auto faceFrom = [](float dx, float dy) -> Direction {
        if (std::abs(dx) + std::abs(dy) < 0.001f) return Direction::None;
        if (std::abs(dx) > std::abs(dy)) return dx > 0 ? Direction::Right : Direction::Left;
        return dy > 0 ? Direction::Down : Direction::Up;
    };

    Direction nf0 = faceFrom(dv0x, dv0y);
    Direction nf1 = faceFrom(dv1x, dv1y);
    if (nf0 != Direction::None) f0_ = nf0;
    if (nf1 != Direction::None) f1_ = nf1;

    lp0x_ = p0x_;
    lp0y_ = p0y_;
    lp1x_ = p1x_;
    lp1y_ = p1y_;

    constexpr float dt      = 1.f / 60.f;
    bool            moving0 = (dv0x * dv0x + dv0y * dv0y) > 0.0001f;
    bool            moving1 = (dv1x * dv1x + dv1y * dv1y) > 0.0001f;

    // Scale — must match server and LevelRenderer
    const int   gridW  = level_.getWidth();
    const int   gridH  = level_.getHeight();
    const float scaleX = window_.getSize().x / static_cast<float>(gridW * 64);
    const float scaleY = window_.getSize().y / static_cast<float>(gridH * 64);
    const float scale  = std::min(scaleX, scaleY) * 0.9f;

    r0_.setFacing(f0_);
    r0_.setPosition(p0x_, p0y_);
    r0_.tick(dt, moving0, scale);
    r1_.setFacing(f1_);
    r1_.setPosition(p1x_, p1y_);
    r1_.tick(dt, moving1, scale);

    GhostRenderer* renderers[4] = {&g0_, &g1_, &g2_, &g3_};
    for (int gi = 0; gi < 4; ++gi) {
        renderers[gi]->setPosition(gx_[gi], gy_[gi]);
        renderers[gi]->setFrightened(ghostFrightened_[gi]);
        renderers[gi]->setFacing(gf_[gi]);
        renderers[gi]->tick(dt, scale);
    }

    // Age popups
    for (auto& p : popups_) std::get<3>(p) += dt;
    popups_.erase(std::remove_if(popups_.begin(), popups_.end(), [](const auto& p) { return std::get<3>(p) > 1.f; }),
                  popups_.end());
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void GameClient::render() {
    window_.clear(sf::Color::Black);
    levelRenderer_.draw(window_, level_);
    r0_.draw(window_);
    r1_.draw(window_);
    g0_.draw(window_);
    g1_.draw(window_);
    g2_.draw(window_);
    g3_.draw(window_);
    renderHUD();
    renderPopups();
    window_.display();
}

void GameClient::renderHUD() {
    if (!fontLoaded_) return;

    scoreText0_.setString(std::string("P1: ") + std::to_string(score0_));
    scoreText1_.setString(std::string("P2: ") + std::to_string(score1_));

    auto b0 = scoreText0_.getLocalBounds();
    auto b1 = scoreText1_.getLocalBounds();
    scoreText0_.setOrigin(b0.left, b0.top);
    scoreText1_.setOrigin(b1.left, b1.top);

    const float margin = 8.f;
    scoreText0_.setPosition(margin, margin);
    scoreText1_.setPosition(window_.getSize().x - b1.width - margin, margin);

    window_.draw(scoreText0_);
    window_.draw(scoreText1_);
}

void GameClient::renderPopups() {
    constexpr float dt = 1.f / 60.f;
    for (auto& p : popups_) {
        std::get<1>(p) -= 20.f * dt;  // float upward

        sf::Text txt;
        if (fontLoaded_) txt.setFont(hudFont_);
        txt.setCharacterSize(20);
        txt.setFillColor(sf::Color::White);
        txt.setOutlineColor(sf::Color::Black);
        txt.setOutlineThickness(2.f);
        txt.setString(std::to_string(std::get<2>(p)) + "!");
        txt.setPosition(std::get<0>(p), std::get<1>(p));
        window_.draw(txt);
    }
}
