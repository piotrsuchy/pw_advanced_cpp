#include "server/GameServer.hpp"

#include <algorithm>
#include <iostream>

static constexpr float WINDOW_W = 800.f;
static constexpr float WINDOW_H = 600.f;

GameServer::GameServer(unsigned short port, int tickHz)
    : port_(port), tickHz_(tickHz), stepDuration_(std::chrono::milliseconds(1000 / tickHz)) {
    sim_.initLevel(1);

    const int gridW = sim_.getLevel().getWidth();
    const int gridH = sim_.getLevel().getHeight();
    float     sx    = WINDOW_W / (gridW * 64.f);
    float     sy    = WINDOW_H / (gridH * 64.f);
    scale_          = std::min(sx, sy) * 0.9f;
    scaledTile_     = 64.f * scale_;
}

void GameServer::run() {
    if (listener_.listen(port_) != sf::Socket::Done) {
        std::cerr << "[SERVER] listen failed on port " << port_ << "\n";
        return;
    }
    listener_.setBlocking(false);
    selector_.add(listener_);

    std::cout << "[SERVER] Listening on port " << port_ << " @ " << tickHz_ << " Hz\n";

    lastTick_ = std::chrono::steady_clock::now();

    while (true) {
        if (selector_.wait(sf::milliseconds(1))) {
            acceptConnections();
            processClientInputs();
        }

        auto now = std::chrono::steady_clock::now();
        if (now - lastTick_ >= stepDuration_) {
            lastTick_ = now;
            tick();
        }
    }
}

void GameServer::acceptConnections() {
    if (!selector_.isReady(listener_)) return;

    int freeIdx = -1;
    for (int i = 0; i < 2; ++i)
        if (!connected_[i]) {
            freeIdx = i;
            break;
        }

    auto sock = std::make_unique<sf::TcpSocket>();
    if (listener_.accept(*sock) != sf::Socket::Done) return;

    if (freeIdx == -1) {
        sock->disconnect();
        return;
    }

    sock->setBlocking(false);
    clients_[freeIdx]   = std::move(sock);
    connected_[freeIdx] = true;
    selector_.add(*clients_[freeIdx]);
    std::cout << "[SERVER] Client connected in slot " << freeIdx << "\n";

    // Sync the full level state immediately
    sf::Packet lvl = buildLevelPacket();
    clients_[freeIdx]->send(lvl);
}

void GameServer::processClientInputs() {
    for (int i = 0; i < 2; ++i) {
        if (!connected_[i]) continue;
        if (!selector_.isReady(*clients_[i])) continue;

        sf::Packet in;
        auto       st = clients_[i]->receive(in);
        if (st == sf::Socket::Done) {
            std::string kind;
            in >> kind;
            if (kind == "INPUT") {
                sf::Uint32 seq;
                sf::Uint8  dir;
                in >> seq >> dir;
                sim_.setDesired(i, static_cast<Direction>(dir));
            }
        } else if (st == sf::Socket::Disconnected) {
            selector_.remove(*clients_[i]);
            clients_[i]->disconnect();
            clients_[i].reset();
            connected_[i] = false;
            std::cout << "[SERVER] Client " << i << " disconnected\n";
        }
    }
}

void GameServer::tick() {
    sim_.step(1.f / tickHz_, scaledTile_, scale_);

    if (sim_.isGameOver()) {
        std::cout << "[SERVER] Game over.\n";
        std::exit(0);
    }
    if (sim_.isLevelComplete()) {
        std::cout << "[SERVER] Level complete!\n";
        std::exit(0);
    }

    std::vector<Simulation::ConsumedPellet>  consumed;
    std::vector<Simulation::EatenGhostEvent> ghostScores;
    sim_.drainConsumed(consumed);
    sim_.drainEatenGhosts(ghostScores);

    sf::Packet snap = buildSnapshotPacket(consumed, ghostScores);

    for (int i = 0; i < 2; ++i) {
        if (!connected_[i]) continue;
        clients_[i]->send(snap);
    }
}

sf::Packet GameServer::buildLevelPacket() const {
    sf::Packet pkt;
    pkt << std::string("LEVEL");
    const int w = sim_.getLevel().getWidth();
    const int h = sim_.getLevel().getHeight();
    pkt << static_cast<sf::Uint16>(w) << static_cast<sf::Uint16>(h);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto      t = sim_.getLevel().getTile(x, y);
            sf::Uint8 v = 0;
            if (t == TileType::Wall)
                v = 1;
            else if (t == TileType::Pellet)
                v = 2;
            else if (t == TileType::PowerPellet)
                v = 3;
            else if (t == TileType::Cherry)
                v = 4;
            pkt << v;
        }
    }
    return pkt;
}

sf::Packet GameServer::buildSnapshotPacket(const std::vector<Simulation::ConsumedPellet>&  consumed,
                                       const std::vector<Simulation::EatenGhostEvent>& ghostScores) const {
    auto s0 = sim_.getPlayerState(0);
    auto s1 = sim_.getPlayerState(1);

    sf::Packet pkt;
    pkt << std::string("SNAPSHOT")
        // Player 0
        << (float)s0.position.x << (float)s0.position.y << (sf::Uint16)s0.score << (sf::Uint8)(s0.powered ? 1 : 0)
        << (sf::Uint8)s0.livesLeft
        << (float)s0.deathTimeLeft
        // Player 1
        << (float)s1.position.x << (float)s1.position.y << (sf::Uint16)s1.score << (sf::Uint8)(s1.powered ? 1 : 0)
        << (sf::Uint8)s1.livesLeft << (float)s1.deathTimeLeft;

    // 4 ghost positions + facings
    for (int gi = 0; gi < 4; ++gi) {
        auto gpos = sim_.getGhostPosition(gi);
        pkt << (float)gpos.x << (float)gpos.y;
    }
    for (int gi = 0; gi < 4; ++gi) {
        pkt << (sf::Uint8)sim_.getGhostFacing(gi);
    }

    // Consumed pellet count + ghost score event count
    pkt << static_cast<sf::Uint16>(consumed.size()) << static_cast<sf::Uint16>(ghostScores.size());

    // Per-ghost active + frightened flags
    for (int gi = 0; gi < 4; ++gi) {
        bool active     = sim_.isGhostActive(gi);
        bool frightened = active && sim_.isGhostFrightened(gi);
        pkt << (sf::Uint8)(active ? 1 : 0) << (sf::Uint8)(frightened ? 1 : 0);
    }

    // Consumed pellet deltas
    for (auto& d : consumed) {
        pkt << static_cast<sf::Uint16>(d.x) << static_cast<sf::Uint16>(d.y)
            << static_cast<sf::Uint8>(d.type == TileType::PowerPellet ? 2 : 1);
    }

    // Ghost-eat score popups
    for (auto& ge : ghostScores) {
        pkt << (float)ge.x << (float)ge.y << (sf::Uint16)ge.points;
    }

    return pkt;
}
