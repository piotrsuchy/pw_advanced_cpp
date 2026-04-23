#include "client/GameClient.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include "shared/AudioCue.hpp"

static constexpr float kPowerPelletFlashWindowSec = 3.f;

static constexpr const char* kMenuItems[]  = {"Play", "Options", "Quit"};
static constexpr std::size_t kNumMenuItems = 3;
static constexpr float       kReadySeconds = 2.2f;
static const sf::Color       kMenuHighlight{100, 200, 255};

namespace {

    bool tryLoadFontFile(sf::Font& font, const std::string& path) {
        if (path.empty()) return false;
        return font.loadFromFile(path);
    }

    /// Tries repo-relative paths (works when cwd is project root or `build/`, etc.)
    bool tryBundledHudFonts(sf::Font& font, std::string* loadedPath) {
        static const char* kFileNames[] = {
            "Roboto-Regular.ttf", "DejaVuSans.ttf", "FreeSans.ttf", "OpenSans-Regular.ttf", "PressStart2P.ttf",
        };
        static const char* kRootDirs[] = {
            "assets/fonts/", "./assets/fonts/", "../assets/fonts/", "../../assets/fonts/", "../../../assets/fonts/",
        };
        for (const char* dir : kRootDirs) {
            for (const char* name : kFileNames) {
                std::filesystem::path p = std::filesystem::path(dir) / name;
                if (tryLoadFontFile(font, p.string())) {
                    if (loadedPath) *loadedPath = p.string();
                    return true;
                }
            }
        }
        return false;
    }

}  // namespace

// ---------------------------------------------------------------------------
// Construction / setup
// ---------------------------------------------------------------------------

GameClient::GameClient(sf::IpAddress serverIp, unsigned short port, int localPlayerIndex)
    : serverIp_(serverIp),
      port_(port),
      localPlayerIndex_(localPlayerIndex),
      window_(sf::VideoMode(800, 600), "Pacman Client") {
    window_.setFramerateLimit(60);
    level_.loadLevel(1);
    setupGhostTextures();
    loadFont();
    audio_.tryLoadBundled();
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
    std::string loadedFrom;

    if (const char* envFont = std::getenv("PACMAN_FONT")) {
        if (tryLoadFontFile(hudFont_, envFont)) {
            fontLoaded_ = true;
            loadedFrom  = envFont;
        }
    }

    if (!fontLoaded_) {
        if (tryBundledHudFonts(hudFont_, &loadedFrom)) fontLoaded_ = true;
    }

    if (!fontLoaded_) {
        const char* systemFonts[] = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
            "/System/Library/Fonts/Supplemental/Times New Roman.ttf",
            "/System/Library/Fonts/Supplemental/Courier New.ttf",
            "/Library/Fonts/Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        };
        for (const char* p : systemFonts) {
            if (tryLoadFontFile(hudFont_, p)) {
                fontLoaded_ = true;
                loadedFrom  = p;
                break;
            }
        }
    }

    if (!fontLoaded_) {
        std::cerr << "[CLIENT] No HUD font found. Set PACMAN_FONT to a .ttf path, or run the client with "
                     "cwd at the repo root / build dir (expects assets/fonts/Roboto-Regular.ttf).\n";
        return;
    }

    std::cerr << "[CLIENT] HUD font: " << loadedFrom << "\n";

    auto setup = [&](sf::Text& t, sf::Color col) {
        t.setFont(hudFont_);
        t.setCharacterSize(24);
        t.setFillColor(col);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(2.f);
    };
    setup(scoreText0_, sf::Color(255, 255, 0));
    setup(scoreText1_, sf::Color(0, 200, 255));
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void GameClient::run() {
    sf::Clock frameClock;

    while (window_.isOpen()) {
        const float dt = std::min(frameClock.restart().asSeconds(), 0.1f);
        if (menuErrorTime_ > 0.f) menuErrorTime_ -= dt;

        processWindowEvents();

        if (flow_ == ClientFlow::MainMenu) {
            render();
            continue;
        }

        // InGame
        if (readyTimer_ > 0.f) readyTimer_ -= dt;

        receivePackets();

        {
            const bool roundLive =
                flow_ == ClientFlow::InGame && haveLevel_ && readyTimer_ <= 0.f && matchOutcome_ == 0;
            audio_.setRoundBgm(roundLive);
        }

        if (flow_ == ClientFlow::InGame && haveLevel_ && readyTimer_ <= 0.f && matchOutcome_ == 0 &&
            !playerReadySent_) {
            sendSimplePacket("PLAYER_READY");
            playerReadySent_ = true;
        }

        sendInput();
        updateAnimations(dt);
        render();
    }
}

// ---------------------------------------------------------------------------
// Per-frame operations
// ---------------------------------------------------------------------------

bool GameClient::connectToServer() {
    socket_.disconnect();
    if (socket_.connect(serverIp_, port_) != sf::Socket::Done) return false;
    socket_.setBlocking(false);
    return true;
}

void GameClient::disconnectToMenu() {
    socket_.disconnect();
    flow_            = ClientFlow::MainMenu;
    subMenu_         = SubMenu::None;
    serverPaused_    = false;
    matchOutcome_    = 0;
    readyTimer_      = 0.f;
    playerReadySent_ = false;
    haveLevel_       = false;
    popups_.clear();
    seq_      = 0;
    lastSent_ = Direction::None;
    input_.clearQueuedDirection();
    audio_.stopAllMusic();
}

void GameClient::processWindowEvents() {
    sf::Event e;
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window_.close();

        if (e.type == sf::Event::KeyPressed) {
            if (subMenu_ == SubMenu::Options) {
                if (e.key.code == sf::Keyboard::Escape) subMenu_ = SubMenu::None;
                continue;
            }

            if (flow_ == ClientFlow::MainMenu) {
                if (e.key.code == sf::Keyboard::Escape) {
                    window_.close();
                } else
                    handleMainMenuKey(e.key.code);
                continue;
            }

            if (flow_ == ClientFlow::InGame) {
                if (e.key.code == sf::Keyboard::R && (matchOutcome_ == 1 || matchOutcome_ == 2)) {
                    sendSimplePacket("RESTART");
                } else if (e.key.code == sf::Keyboard::M && (matchOutcome_ == 1 || matchOutcome_ == 2)) {
                    disconnectToMenu();
                } else if (e.key.code == sf::Keyboard::P || e.key.code == sf::Keyboard::Escape) {
                    if (serverPaused_ || (matchOutcome_ == 0 && readyTimer_ <= 0.f)) sendSimplePacket("PAUSE");
                } else if (canPlayerSteer()) {
                    input_.handleEvent(e);
                }
            }
        }
    }
}

void GameClient::handleMainMenuKey(sf::Keyboard::Key k) {
    if (k == sf::Keyboard::Up) {
        mainMenuIndex_ = (mainMenuIndex_ + static_cast<int>(kNumMenuItems) - 1) % static_cast<int>(kNumMenuItems);
    } else if (k == sf::Keyboard::Down) {
        mainMenuIndex_ = (mainMenuIndex_ + 1) % static_cast<int>(kNumMenuItems);
    } else if (k == sf::Keyboard::Return) {
        if (mainMenuIndex_ == 0) {
            if (connectToServer()) {
                flow_       = ClientFlow::InGame;
                haveLevel_  = false;
                readyTimer_ = 0.f;
            } else {
                menuError_     = "Connection failed. Is the server running?";
                menuErrorTime_ = 5.f;
            }
        } else if (mainMenuIndex_ == 1) {
            subMenu_ = SubMenu::Options;
        } else
            window_.close();
    }
}

bool GameClient::canPlayerSteer() const {
    return haveLevel_ && readyTimer_ <= 0.f && !serverPaused_ && matchOutcome_ == 0;
}

void GameClient::sendInput() {
    if (flow_ != ClientFlow::InGame) return;
    if (matchOutcome_ != 0) return;
    if (!canPlayerSteer()) return;

    Direction d = input_.popQueuedDirection();
    if (d == Direction::None || d == lastSent_) return;
    lastSent_ = d;
    sf::Packet pkt;
    pkt << std::string("INPUT") << seq_++ << static_cast<sf::Uint8>(d);
    socket_.send(pkt);
}

void GameClient::sendSimplePacket(const char* kind) {
    if (flow_ != ClientFlow::InGame) return;
    sf::Packet pkt;
    pkt << std::string(kind);
    socket_.send(pkt);
}

void GameClient::receivePackets() {
    if (flow_ != ClientFlow::InGame) return;

    while (true) {
        sf::Packet pkt;
        const auto st = socket_.receive(pkt);
        if (st == sf::Socket::NotReady) break;
        if (st != sf::Socket::Done) {
            disconnectToMenu();
            menuError_     = "Disconnected from server.";
            menuErrorTime_ = 5.f;
            break;
        }
        std::string kind;
        pkt >> kind;
        if (kind == "SNAPSHOT")
            processSnapshot(pkt);
        else if (kind == "LEVEL")
            processLevel(pkt);
    }
}

void GameClient::processSnapshot(sf::Packet& pkt) {
    const unsigned oldL0 = lives0_, oldL1 = lives1_;
    bool           oldAnyFrightened = false;
    for (int i = 0; i < 4; ++i) {
        if (ghostActive_[i] && ghostFrightened_[i]) oldAnyFrightened = true;
    }

    sf::Uint16 s0, s1;
    sf::Uint8  pw0, pw1, l0, l1;
    float      dt0, dt1;
    pkt >> p0x_ >> p0y_ >> s0 >> pw0 >> l0 >> dt0 >> powerTime0_;
    pkt >> p1x_ >> p1y_ >> s1 >> pw1 >> l1 >> dt1 >> powerTime1_;

    score0_ = s0;
    score1_ = s1;
    lives0_ = l0;
    lives1_ = l1;
    if (l0 < oldL0 || l1 < oldL1) audio_.playCue(AudioCue::PacmanDeath);

    pow0_ = (pw0 != 0);
    pow1_ = (pw1 != 0);

    r0_.setDeathTimeLeft(dt0);
    r1_.setDeathTimeLeft(dt1);

    for (int gi = 0; gi < 4; ++gi) pkt >> gx_[gi] >> gy_[gi];
    for (int gi = 0; gi < 4; ++gi) {
        sf::Uint8 f;
        pkt >> f;
        gf_[gi] = static_cast<Direction>(f);
    }

    sf::Uint16 nPellets, nGhostScores;
    pkt >> nPellets >> nGhostScores;

    for (int gi = 0; gi < 4; ++gi) {
        sf::Uint8 a, fr;
        pkt >> a >> fr;
        ghostActive_[gi]     = (a != 0);
        ghostFrightened_[gi] = (fr != 0);
    }

    bool newAnyFrightened = false;
    for (int i = 0; i < 4; ++i) {
        if (ghostActive_[i] && ghostFrightened_[i]) newAnyFrightened = true;
    }
    if (newAnyFrightened != oldAnyFrightened) audio_.setFrightenedLoop(newAnyFrightened);

    for (sf::Uint16 k = 0; k < nPellets; ++k) {
        sf::Uint16 cx, cy;
        sf::Uint8  t;
        pkt >> cx >> cy >> t;
        if (t == 1) audio_.playCue(AudioCue::PelletChomp);
        if (t == 2) audio_.playCue(AudioCue::PowerPellet);
        if (t == 1 || t == 2) level_.setTile(cx, cy, TileType::Empty);
    }

    for (sf::Uint16 k = 0; k < nGhostScores; ++k) {
        float      ex, ey;
        sf::Uint16 pts;
        pkt >> ex >> ey >> pts;
        audio_.playCue(AudioCue::GhostEaten);
        popups_.emplace_back(ex, ey, static_cast<int>(pts), 0.f);
    }

    sf::Uint8 pflag, outc;
    pkt >> pflag >> outc;
    serverPaused_ = (pflag != 0);
    matchOutcome_ = outc;
    if (outc != 0) audio_.setFrightenedLoop(false);
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
    haveLevel_       = true;
    readyTimer_      = kReadySeconds;
    playerReadySent_ = false;
    lastSent_        = Direction::None;
    input_.clearQueuedDirection();
    matchOutcome_ = 0;
    serverPaused_ = false;
    audio_.stopAllMusic();
    audio_.playLevelIntro();
}

void GameClient::updateAnimations(float dt) {
    if (flow_ != ClientFlow::InGame || !haveLevel_) return;

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

    bool moving0 = (dv0x * dv0x + dv0y * dv0y) > 0.0001f;
    bool moving1 = (dv1x * dv1x + dv1y * dv1y) > 0.0001f;

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

    const float maxPower = std::max(powerTime0_, powerTime1_);
    const bool  endFlash = maxPower > 0.f && maxPower <= kPowerPelletFlashWindowSec;

    GhostRenderer* renderers[4] = {&g0_, &g1_, &g2_, &g3_};
    for (int gi = 0; gi < 4; ++gi) {
        renderers[gi]->setPosition(gx_[gi], gy_[gi]);
        const bool fr = ghostFrightened_[gi] && ghostActive_[gi];
        renderers[gi]->setFrightened(fr);
        renderers[gi]->setFrightenedEndFlash(endFlash && fr);
        renderers[gi]->setFacing(gf_[gi]);
        renderers[gi]->tick(dt, scale);
    }

    for (auto& p : popups_) std::get<3>(p) += dt;
    popups_.erase(std::remove_if(popups_.begin(), popups_.end(), [](const auto& p) { return std::get<3>(p) > 1.f; }),
                  popups_.end());
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void GameClient::drawCenteredText(const sf::Text& t, float y) {
    sf::Text copy = t;
    auto     b    = copy.getLocalBounds();
    copy.setOrigin(b.width * 0.5f, b.height * 0.5f);
    copy.setPosition(window_.getSize().x * 0.5f, y);
    window_.draw(copy);
}

void GameClient::renderDimBackground(float a) {
    sf::RectangleShape overlay{sf::Vector2f(window_.getSize().x, window_.getSize().y)};
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(a * 255.f)));
    window_.draw(overlay);
}

void GameClient::renderStatusLine(const char* line) {
    window_.clear(sf::Color::Black);
    sf::Text t;
    if (fontLoaded_) t.setFont(hudFont_);
    t.setString(line);
    t.setCharacterSize(28);
    t.setFillColor(sf::Color::White);
    drawCenteredText(t, window_.getSize().y * 0.5f);
    if (!menuError_.empty() && menuErrorTime_ > 0.f) {
        t.setString(menuError_);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(255, 120, 120));
        drawCenteredText(t, window_.getSize().y * 0.5f + 40.f);
    }
    window_.display();
}

void GameClient::drawMainMenu() {
    sf::Text title;
    if (fontLoaded_) title.setFont(hudFont_);
    title.setString("PAC-MAN");
    title.setCharacterSize(56);
    title.setFillColor(sf::Color(255, 220, 0));
    title.setOutlineColor(sf::Color(0, 0, 0));
    title.setOutlineThickness(3.f);
    drawCenteredText(title, 90.f);

    for (std::size_t i = 0; i < kNumMenuItems; ++i) {
        sf::Text item;
        if (fontLoaded_) item.setFont(hudFont_);
        item.setString(kMenuItems[i]);
        item.setCharacterSize(30);
        const bool sel = (static_cast<int>(i) == mainMenuIndex_);
        item.setFillColor(sel ? kMenuHighlight : sf::Color::White);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        drawCenteredText(item, 200.f + static_cast<float>(i) * 44.f);
    }

    {
        sf::Text hint;
        if (fontLoaded_) hint.setFont(hudFont_);
        hint.setString("Up/Down  Enter  |  Esc quit");
        hint.setCharacterSize(16);
        hint.setFillColor(sf::Color(200, 200, 200));
        drawCenteredText(hint, static_cast<float>(window_.getSize().y) - 40.f);
    }

    if (menuErrorTime_ > 0.f && !menuError_.empty()) {
        sf::Text err;
        if (fontLoaded_) err.setFont(hudFont_);
        err.setString(menuError_);
        err.setCharacterSize(18);
        err.setFillColor(sf::Color(255, 100, 100));
        drawCenteredText(err, static_cast<float>(window_.getSize().y) - 80.f);
    }
}

void GameClient::drawOptionsOverlay() {
    if (!fontLoaded_) return;
    renderDimBackground(0.55f);
    std::string ipStr = serverIp_.toString();
    sf::Text    a, b, c, back;
    a.setFont(hudFont_);
    a.setString("Options");
    a.setCharacterSize(36);
    a.setFillColor(sf::Color::White);
    a.setOutlineThickness(2.f);
    a.setOutlineColor(sf::Color::Black);
    drawCenteredText(a, 100.f);
    b.setFont(hudFont_);
    b.setString("Server: " + ipStr + "  port " + std::to_string(port_));
    b.setCharacterSize(20);
    b.setFillColor(sf::Color(220, 220, 255));
    b.setOutlineThickness(1.f);
    b.setOutlineColor(sf::Color::Black);
    drawCenteredText(b, 200.f);
    c.setFont(hudFont_);
    c.setString("Set host with:  pacman_client --ip <addr> --port <n>");
    c.setCharacterSize(16);
    c.setFillColor(sf::Color(180, 180, 200));
    drawCenteredText(c, 250.f);
    back.setFont(hudFont_);
    back.setString("Back - Esc");
    back.setCharacterSize(22);
    back.setFillColor(sf::Color(200, 255, 200));
    drawCenteredText(back, 400.f);
}

void GameClient::drawReadyOverlay() {
    if (!fontLoaded_) return;
    renderDimBackground(0.35f);
    sf::Text t;
    t.setFont(hudFont_);
    t.setString("READY!");
    t.setCharacterSize(64);
    t.setFillColor(sf::Color(255, 255, 100));
    t.setOutlineColor(sf::Color(0, 0, 80));
    t.setOutlineThickness(3.f);
    drawCenteredText(t, window_.getSize().y * 0.45f);
}

void GameClient::drawPauseOverlay() {
    if (!fontLoaded_) return;
    renderDimBackground(0.5f);
    sf::Text t, u;
    t.setFont(hudFont_);
    t.setString("PAUSED");
    t.setCharacterSize(48);
    t.setFillColor(sf::Color(255, 255, 200));
    t.setOutlineThickness(2.f);
    t.setOutlineColor(sf::Color::Black);
    drawCenteredText(t, window_.getSize().y * 0.42f);
    u.setFont(hudFont_);
    u.setString("P or Esc - resume");
    u.setCharacterSize(20);
    u.setFillColor(sf::Color(200, 200, 255));
    u.setOutlineThickness(1.f);
    u.setOutlineColor(sf::Color::Black);
    drawCenteredText(u, window_.getSize().y * 0.55f);
}

void GameClient::drawEndScreen(bool victory) {
    if (!fontLoaded_) return;
    renderDimBackground(0.65f);
    sf::Text title, s, hint;
    title.setFont(hudFont_);
    title.setString(victory ? "LEVEL COMPLETE" : "GAME OVER");
    title.setCharacterSize(42);
    title.setFillColor(victory ? sf::Color(100, 255, 150) : sf::Color(255, 100, 100));
    title.setOutlineThickness(3.f);
    title.setOutlineColor(sf::Color::Black);
    drawCenteredText(title, 150.f);
    s.setFont(hudFont_);
    s.setString(std::string("P1 score ") + std::to_string(score0_) + "    P2 score " + std::to_string(score1_));
    s.setCharacterSize(22);
    s.setFillColor(sf::Color::White);
    drawCenteredText(s, 250.f);
    {
        int my = localPlayerIndex_ == 0 ? static_cast<int>(lives0_) : static_cast<int>(lives1_);
        hint.setFont(hudFont_);
        if (victory)
            hint.setString("Nice! Press R to play again, M for main menu.");
        else
            hint.setString((my <= 0 ? std::string("You are out of lives. ") : std::string()) +
                           "R - retry   M - main menu");
        hint.setCharacterSize(20);
        hint.setFillColor(sf::Color(230, 230, 255));
        drawCenteredText(hint, 340.f);
    }
}

void GameClient::render() {
    if (flow_ == ClientFlow::MainMenu) {
        window_.clear(sf::Color(0, 0, 32));
        drawMainMenu();
        if (subMenu_ == SubMenu::Options) drawOptionsOverlay();
        window_.display();
        return;
    }

    if (flow_ == ClientFlow::InGame && !haveLevel_) {
        renderStatusLine("Waiting for level data...");
        return;
    }

    if (readyTimer_ > 0.f) {
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
        drawReadyOverlay();
        window_.display();
        return;
    }

    if (flow_ == ClientFlow::InGame) {
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

        if (serverPaused_ && matchOutcome_ == 0) {
            drawPauseOverlay();
        } else if (matchOutcome_ == 1) {
            drawEndScreen(false);
        } else if (matchOutcome_ == 2) {
            drawEndScreen(true);
        }
        window_.display();
    }
}

void GameClient::renderHUD() {
    if (!fontLoaded_) return;

    scoreText0_.setString(std::string("P1: ") + std::to_string(score0_) + "   Lives: " + std::to_string(lives0_));
    scoreText1_.setString(std::string("P2: ") + std::to_string(score1_) + "   Lives: " + std::to_string(lives1_));

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
    for (auto& p : popups_) {
        constexpr float kDt = 1.f / 60.f;
        std::get<1>(p) -= 20.f * kDt;
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
