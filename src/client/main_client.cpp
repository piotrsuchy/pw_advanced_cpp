#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#include "core/InputManager.hpp"
#include "core/LevelManager.hpp"
#include "graphics/LevelRenderer.hpp"

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
    LevelRenderer renderer;

    sf::CircleShape p0(10.f);
    p0.setFillColor(sf::Color::Yellow);
    sf::CircleShape p1(10.f);
    p1.setFillColor(sf::Color::Cyan);

    InputManager input;
    Direction    lastSent = Direction::None;
    sf::Uint32   seq      = 0;

    float p0x = 120.f, p0y = 120.f, p1x = 680.f, p1y = 480.f;

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
                in >> p0x >> p0y >> p1x >> p1y;
            }
        }

        p0.setPosition(p0x, p0y);
        p1.setPosition(p1x, p1y);

        window.clear(sf::Color::Black);
        renderer.draw(window, level);
        window.draw(p0);
        window.draw(p1);
        window.display();
    }

    return 0;
}
