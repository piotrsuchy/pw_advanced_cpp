#include <SFML/Network.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "shared/Simulation.hpp"

static constexpr float WINDOW_W = 800.f;
static constexpr float WINDOW_H = 600.f;

int main(int argc, char** argv) {
    unsigned short port   = 54000;
    int            tickHz = 60;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--port" || arg == "-p") && i + 1 < argc)
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        else if ((arg == "--tick" || arg == "-t") && i + 1 < argc)
            tickHz = std::max(1, std::stoi(argv[++i]));
    }

    std::cout << "[SERVER] Start on port " << port << ", tick=" << tickHz << " Hz" << std::endl;

    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "[SERVER] listen failed" << std::endl;
        return 1;
    }
    listener.setBlocking(false);

    std::unique_ptr<sf::TcpSocket> client[2];
    bool                           connected[2] = {false, false};
    sf::SocketSelector             selector;
    selector.add(listener);

    Simulation sim;
    sim.initLevel(1);

    // Derive consistent scaledTileSize/scale like client renderer (roughly)
    const int gridW      = sim.getLevel().getWidth();
    const int gridH      = sim.getLevel().getHeight();
    float     scaleX     = WINDOW_W / (gridW * 64.f);
    float     scaleY     = WINDOW_H / (gridH * 64.f);
    float     scale      = std::min(scaleX, scaleY) * 0.9f;
    float     scaledTile = 64.f * scale;

    auto lastTick = std::chrono::steady_clock::now();
    auto step     = std::chrono::milliseconds(1000 / tickHz);

    while (true) {
        if (selector.wait(sf::milliseconds(1))) {
            if (selector.isReady(listener)) {
                int freeIdx = -1;
                for (int i = 0; i < 2; ++i)
                    if (!connected[i]) {
                        freeIdx = i;
                        break;
                    }
                auto sock = std::make_unique<sf::TcpSocket>();
                if (listener.accept(*sock) == sf::Socket::Done) {
                    if (freeIdx != -1) {
                        sock->setBlocking(false);
                        client[freeIdx]    = std::move(sock);
                        connected[freeIdx] = true;
                        selector.add(*client[freeIdx]);
                        std::cout << "[SERVER] Client connected in slot " << freeIdx << std::endl;
                    } else {
                        sock->disconnect();
                    }
                }
            }
            for (int i = 0; i < 2; ++i) {
                if (!connected[i]) continue;
                if (selector.isReady(*client[i])) {
                    sf::Packet in;
                    auto       st = client[i]->receive(in);
                    if (st == sf::Socket::Done) {
                        std::string kind;
                        in >> kind;
                        if (kind == "INPUT") {
                            sf::Uint32 seq;
                            sf::Uint8  dir;
                            in >> seq >> dir;
                            sim.setDesired(i, static_cast<Direction>(dir));
                            std::cout << "[SERVER] INPUT from " << i << " seq=" << seq << " dir=" << (int)dir
                                      << std::endl;
                        }
                    } else if (st == sf::Socket::Disconnected) {
                        selector.remove(*client[i]);
                        client[i]->disconnect();
                        client[i].reset();
                        connected[i] = false;
                        std::cout << "[SERVER] Client " << i << " disconnected" << std::endl;
                    }
                }
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (now - lastTick >= step) {
            lastTick = now;
            sim.step(1.0f / tickHz, scaledTile, scale);

            for (int i = 0; i < 2; ++i) {
                if (!connected[i]) continue;
                auto       p0 = sim.getPlayerState(0).position;
                auto       p1 = sim.getPlayerState(1).position;
                sf::Packet out;
                out << std::string("SNAPSHOT") << (float)p0.x << (float)p0.y << (float)p1.x << (float)p1.y;
                client[i]->send(out);
            }
        }
    }
    return 0;
}
