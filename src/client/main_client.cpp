#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <cstdlib>
#include <iostream>

#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"
#include "graphics/GhostRenderer.hpp"
#include "graphics/LevelRenderer.hpp"
#include "graphics/PacmanRenderer.hpp"

int main(int argc, char** argv) {
    sf::IpAddress  ip   = sf::IpAddress::LocalHost;
    unsigned short port = 54000;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "--ip" || a == "-i") && i + 1 < argc)
            ip = sf::IpAddress(argv[++i]);
        else if ((a == "--port" || a == "-p") && i + 1 < argc)
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
    }

    sf::TcpSocket socket;
    if (socket.connect(ip, port) != sf::Socket::Done) {
        std::cerr << "[CLIENT] connect failed\n";
        return 1;
    }
    socket.setBlocking(false);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Pacman Client");
    window.setFramerateLimit(60);

    LevelManager level;
    level.loadLevel(1);
    LevelRenderer  renderer;
    PacmanRenderer r0, r1;
    GhostRenderer  g0, g1, g2, g3;
    // textures for Blinky
    g0.setDirectionalTextures("assets/textures/ghosts/blinky_up.png", "assets/textures/ghosts/blinky_down.png",
                              "assets/textures/ghosts/blinky_left.png", "assets/textures/ghosts/blinky_right.png");
    // textures for Pinky
    g1.setDirectionalTextures("assets/textures/ghosts/pinky_up.png", "assets/textures/ghosts/pinky_down.png",
                              "assets/textures/ghosts/pinky_left.png", "assets/textures/ghosts/pinky_right.png");
    // textures for Inky
    g2.setDirectionalTextures("assets/textures/ghosts/inky_up.png", "assets/textures/ghosts/inky_down.png",
                              "assets/textures/ghosts/inky_left.png", "assets/textures/ghosts/inky_right.png");
    // textures for Clyde
    g3.setDirectionalTextures("assets/textures/ghosts/clyde_up.png", "assets/textures/ghosts/clyde_down.png",
                              "assets/textures/ghosts/clyde_left.png", "assets/textures/ghosts/clyde_right.png");

    InputManager input;
    Direction    lastSent = Direction::None;
    sf::Uint32   seq      = 0;

    float     p0x = 120.f, p0y = 120.f, p1x = 680.f, p1y = 480.f;
    float     gx0 = 400.f, gy0 = 300.f, gx1 = 400.f, gy1 = 300.f, gx2 = 400.f, gy2 = 300.f, gx3 = 400.f, gy3 = 300.f;
    Direction gf0 = Direction::Left, gf1 = Direction::Left, gf2 = Direction::Left, gf3 = Direction::Left;
    uint16_t  score0 = 0, score1 = 0;
    bool      pow0 = false, pow1 = false;
    Direction f0 = Direction::Right, f1 = Direction::Left;

    // HUD font and texts - try loading from some usual positions
    // if the env variable is not provided
    sf::Font hudFont;
    bool     fontLoaded = false;
    {
        // allow override via env var PACMAN_FONT
        if (const char* envFont = std::getenv("PACMAN_FONT")) {
            fontLoaded = hudFont.loadFromFile(envFont);
        }
        // prefer bundled fonts in assets/fonts (OS-independent)
        if (!fontLoaded) {
            const char* assetFonts[] = {"assets/fonts/DejaVuSans.ttf", "assets/fonts/FreeSans.ttf",
                                        "assets/fonts/OpenSans-Regular.ttf", "assets/fonts/PressStart2P.ttf"};
            for (const char* path : assetFonts) {
                if (hudFont.loadFromFile(path)) {
                    fontLoaded = true;
                    break;
                }
            }
        }
        // fallback to common Linux/macOS paths if assets missing
        if (!fontLoaded) {
            const char* systemFonts[] = {"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                                         "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                                         "/Library/Fonts/Arial.ttf",
                                         "/System/Library/Fonts/Supplemental/Arial.ttf",
                                         "/System/Library/Fonts/Supplemental/Helvetica.ttf",
                                         "/System/Library/Fonts/Supplemental/Tahoma.ttf"};
            for (const char* path : systemFonts) {
                if (hudFont.loadFromFile(path)) {
                    fontLoaded = true;
                    break;
                }
            }
        }
        if (!fontLoaded) {
            std::cerr << "[CLIENT] Warning: Could not load a font (set PACMAN_FONT or add assets/fonts/*.ttf)."
                      << std::endl;
        }
    }
    sf::Text scoreText0;
    sf::Text scoreText1;
    if (fontLoaded) {
        scoreText0.setFont(hudFont);
        scoreText1.setFont(hudFont);
        scoreText0.setCharacterSize(24);
        scoreText1.setCharacterSize(24);
        scoreText0.setFillColor(sf::Color(255, 255, 0));  // yellow-ish for P1
        scoreText1.setFillColor(sf::Color(0, 200, 255));  // cyan-ish for P2
        scoreText0.setOutlineThickness(2.f);
        scoreText1.setOutlineThickness(2.f);
        scoreText0.setOutlineColor(sf::Color::Black);
        scoreText1.setOutlineColor(sf::Color::Black);
    }

    // ephemeral score popups (x, y, points, time)
    std::vector<std::tuple<float, float, int, float>> popups;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            input.handleEvent(e);
        }

        Direction d = input.popQueuedDirection();
        if (d != Direction::None && d != lastSent) {
            lastSent = d;
            sf::Packet pkt;
            pkt << std::string("INPUT") << seq++ << static_cast<sf::Uint8>(d);
            socket.send(pkt);
        }

        // receive latest snapshot in this frame
        while (true) {
            sf::Packet in;
            auto       st = socket.receive(in);
            if (st != sf::Socket::Done) break;
            std::string kind;
            in >> kind;
            if (kind == "SNAPSHOT") {
                sf::Uint16 s0, s1;
                sf::Uint8  pw0, pw1;
                sf::Uint16 n;
                // Extend snapshot with 4 ghost positions and facings
                sf::Uint8 tgf0, tgf1, tgf2, tgf3;
                sf::Uint8 l0, l1;
                float     dt0, dt1;
                in >> p0x >> p0y >> s0 >> pw0 >> l0 >> dt0 >> p1x >> p1y >> s1 >> pw1 >> l1 >> dt1;
                in >> gx0 >> gy0 >> gx1 >> gy1 >> gx2 >> gy2 >> gx3 >> gy3 >> tgf0 >> tgf1 >> tgf2 >> tgf3;
                gf0 = static_cast<Direction>(tgf0);
                gf1 = static_cast<Direction>(tgf1);
                gf2 = static_cast<Direction>(tgf2);
                gf3 = static_cast<Direction>(tgf3);
                sf::Uint16 ng;
                in >> n >> ng;
                score0 = s0;
                score1 = s1;
                pow0   = (pw0 != 0);
                pow1   = (pw1 != 0);
                // Lives and death timers (client could show HUD or trigger animation)
                int   lives0 = l0, lives1 = l1;
                float dead0 = dt0, dead1 = dt1;
                r0.setDeathTimeLeft(dead0);
                r1.setDeathTimeLeft(dead1);
                for (sf::Uint16 k = 0; k < n; ++k) {
                    sf::Uint16 cx, cy;
                    sf::Uint8  t;
                    in >> cx >> cy >> t;
                    if (t == 1 || t == 2) level.setTile(cx, cy, TileType::Empty);
                }
                for (sf::Uint16 k = 0; k < ng; ++k) {
                    float      ex, ey;
                    sf::Uint16 pts;
                    in >> ex >> ey >> pts;
                    popups.emplace_back(ex, ey, (int)pts, 0.f);
                }
            } else if (kind == "LEVEL") {
                // Full level sync from server
                sf::Uint16 w, h;
                in >> w >> h;
                // Rebuild level grid from packet tiles
                for (int y = 0; y < static_cast<int>(h); ++y) {
                    for (int x = 0; x < static_cast<int>(w); ++x) {
                        sf::Uint8 tv;
                        in >> tv;
                        TileType t = TileType::Empty;
                        if (tv == 1)
                            t = TileType::Wall;
                        else if (tv == 2)
                            t = TileType::Pellet;
                        else if (tv == 3)
                            t = TileType::PowerPellet;
                        else if (tv == 4)
                            t = TileType::Cherry;
                        level.setTile(x, y, t);
                    }
                }
            }
        }

        // Estimate facing and movement for animation
        static float lp0x = p0x, lp0y = p0y, lp1x = p1x, lp1y = p1y;
        float        dv0x = p0x - lp0x, dv0y = p0y - lp0y;
        float        dv1x = p1x - lp1x, dv1y = p1y - lp1y;
        auto         faceFrom = [](float dx, float dy) {
            if (std::abs(dx) + std::abs(dy) < 0.001f) return Direction::None;
            if (std::abs(dx) > std::abs(dy)) return dx > 0 ? Direction::Right : Direction::Left;
            return dy > 0 ? Direction::Down : Direction::Up;
        };
        Direction nf0 = faceFrom(dv0x, dv0y);
        if (nf0 != Direction::None) f0 = nf0;
        Direction nf1 = faceFrom(dv1x, dv1y);
        if (nf1 != Direction::None) f1 = nf1;
        lp0x = p0x;
        lp0y = p0y;
        lp1x = p1x;
        lp1y = p1y;

        bool moving0 = (dv0x * dv0x + dv0y * dv0y) > 0.0001f;
        bool moving1 = (dv1x * dv1x + dv1y * dv1y) > 0.0001f;

        // Compute scale to match LevelRenderer
        int   gridW  = level.getWidth();
        int   gridH  = level.getHeight();
        int   totalW = gridW * 64;
        int   totalH = gridH * 64;
        float scaleX = window.getSize().x / static_cast<float>(totalW);
        float scaleY = window.getSize().y / static_cast<float>(totalH);
        float scale  = std::min(scaleX, scaleY) * 0.9f;

        r0.setFacing(f0);
        r0.setPosition(p0x, p0y);
        r0.tick(1.0f / 60.0f, moving0, scale);
        r1.setFacing(f1);
        r1.setPosition(p1x, p1y);
        r1.tick(1.0f / 60.0f, moving1, scale);
        g0.setPosition(gx0, gy0);
        g1.setPosition(gx1, gy1);
        g2.setPosition(gx2, gy2);
        g3.setPosition(gx3, gy3);
        bool frightened = pow0 || pow1;
        g0.setFrightened(frightened);
        g1.setFrightened(frightened);
        g2.setFrightened(frightened);
        g3.setFrightened(frightened);
        // server-authoritative ghost facing
        g0.setFacing(static_cast<Direction>(gf0));
        g1.setFacing(static_cast<Direction>(gf1));
        g2.setFacing(static_cast<Direction>(gf2));
        g3.setFacing(static_cast<Direction>(gf3));
        g0.tick(1.0f / 60.0f, scale);
        g1.tick(1.0f / 60.0f, scale);
        g2.tick(1.0f / 60.0f, scale);
        g3.tick(1.0f / 60.0f, scale);

        window.clear(sf::Color::Black);
        renderer.draw(window, level);
        r0.draw(window);
        r1.draw(window);
        g0.draw(window);
        g1.draw(window);
        g2.draw(window);
        g3.draw(window);

        // Draw HUD scores
        if (fontLoaded) {
            scoreText0.setString(std::string("P1: ") + std::to_string(score0));
            scoreText1.setString(std::string("P2: ") + std::to_string(score1));

            // Normalize origins to top-left of text bounds
            auto b0 = scoreText0.getLocalBounds();
            auto b1 = scoreText1.getLocalBounds();
            scoreText0.setOrigin(b0.left, b0.top);
            scoreText1.setOrigin(b1.left, b1.top);

            const float margin = 8.f;
            scoreText0.setPosition(margin, margin);
            scoreText1.setPosition(window.getSize().x - b1.width - margin, margin);

            window.draw(scoreText0);
            window.draw(scoreText1);
        }

        // Render ephemeral ghost-score popups (float upward and fade out over 1s)
        for (auto& p : popups) {
            std::get<1>(p) -= 20.f * (1.0f / 60.0f);
            std::get<3>(p) += 1.0f / 60.0f;
        }
        popups.erase(std::remove_if(popups.begin(), popups.end(), [](auto& p) { return std::get<3>(p) > 1.0f; }),
                     popups.end());
        for (auto& p : popups) {
            sf::Text txt;
            if (fontLoaded) txt.setFont(hudFont);
            txt.setCharacterSize(20);
            txt.setFillColor(sf::Color::White);
            txt.setOutlineColor(sf::Color::Black);
            txt.setOutlineThickness(2.f);
            txt.setString(std::to_string(std::get<2>(p)) + "!");
            txt.setPosition(std::get<0>(p), std::get<1>(p));
            window.draw(txt);
        }

        window.display();
    }

    return 0;
}
