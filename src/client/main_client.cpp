#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"
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

    InputManager input;
    Direction    lastSent = Direction::None;
    sf::Uint32   seq      = 0;

    float     p0x = 120.f, p0y = 120.f, p1x = 680.f, p1y = 480.f;
    uint16_t  score0 = 0, score1 = 0;
    bool      pow0 = false, pow1 = false;
    Direction f0 = Direction::Right, f1 = Direction::Left;

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
                in >> p0x >> p0y >> s0 >> pw0 >> p1x >> p1y >> s1 >> pw1 >> n;
                score0 = s0;
                score1 = s1;
                pow0   = (pw0 != 0);
                pow1   = (pw1 != 0);
                for (sf::Uint16 k = 0; k < n; ++k) {
                    sf::Uint16 cx, cy;
                    sf::Uint8  t;
                    in >> cx >> cy >> t;
                    if (t == 1 || t == 2) level.setTile(cx, cy, TileType::Empty);
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

        window.clear(sf::Color::Black);
        renderer.draw(window, level);
        r0.draw(window);
        r1.draw(window);
        window.display();
    }

    return 0;
}
