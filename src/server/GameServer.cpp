#include "server/GameServer.hpp"

#include <algorithm>
#include <iostream>

#include "core/ICollectible.hpp"

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
    clients_[freeIdx]     = std::move(sock);
    connected_[freeIdx]   = true;
    playerReady_[freeIdx] = false;
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
            } else if (kind == "PLAYER_READY") {
                if (!playerReady_[i]) {
                    playerReady_[i] = true;
                    tryStartRoundAfterReady();
                }
            } else if (kind == "PAUSE" && match_.phase() == MatchPhase::Playing) {
                simPaused_ = !simPaused_;
            } else if (kind == "RESTART" && (match_.isGameOver() || match_.isLevelComplete())) {
                resetMatch();
            }
        } else if (st == sf::Socket::Disconnected) {
            selector_.remove(*clients_[i]);
            clients_[i]->disconnect();
            clients_[i].reset();
            onClientDisconnected(i);
        }
    }
}

void GameServer::resetMatch() {
    simPaused_ = false;
    sim_.resetForNewMatch(1);
    match_.resetToWaiting();
    playerReady_ = {};
    std::cout << "[SERVER] Match restarted (waiting for PLAYER_READY).\n";
    broadcastLevelToAll();
}

void GameServer::tryStartRoundAfterReady() {
    if (match_.phase() != MatchPhase::Waiting) return;

    bool anyConnected = false;
    for (int j = 0; j < 2; ++j) {
        if (!connected_[j]) continue;
        anyConnected = true;
        if (!playerReady_[j]) return;
    }
    if (!anyConnected) return;

    match_.beginFromWaiting();
    std::cout << "[SERVER] All connected players ready — round live.\n";
}

void GameServer::onClientDisconnected(int slot) {
    connected_[slot]   = false;
    playerReady_[slot] = false;
    std::cout << "[SERVER] Client " << slot << " disconnected\n";

    if (!connected_[0] && !connected_[1]) {
        simPaused_ = false;
        sim_.resetForNewMatch(1);
        match_.resetToWaiting();
        playerReady_ = {};
        std::cout << "[SERVER] No clients — lobby reset.\n";
    } else {
        tryStartRoundAfterReady();
    }
}

void GameServer::broadcastLevelToAll() {
    sf::Packet lvl = buildLevelPacket();
    for (int i = 0; i < 2; ++i) {
        if (!connected_[i]) continue;
        clients_[i]->send(lvl);
    }
}

void GameServer::tick() {
    const bool anyClient     = connected_[0] || connected_[1];
    const bool inActiveRound = anyClient && (match_.phase() == MatchPhase::Playing) && !simPaused_;
    if (inActiveRound) {
        sim_.step(1.f / tickHz_, scaledTile_, scale_);
        match_.updateAfterStep(sim_);
    }

    static bool loggedGo = false, loggedWin = false;
    if (match_.isGameOver() && !loggedGo) {
        std::cout << "[SERVER] Game over (send RESTART to play again).\n";
        loggedGo = true;
    }
    if (match_.isLevelComplete() && !loggedWin) {
        std::cout << "[SERVER] Level complete (send RESTART to play again).\n";
        loggedWin = true;
    }
    if (match_.phase() == MatchPhase::Playing) {
        loggedGo  = false;
        loggedWin = false;
    }

    std::vector<Simulation::ConsumedPellet>  consumed;
    std::vector<Simulation::EatenGhostEvent> ghostScores;
    std::vector<Simulation::GridTileUpdate>  gridUpdates;
    std::vector<Simulation::EatenGhostEvent> fruitPopups;
    sim_.drainConsumed(consumed);
    sim_.drainEatenGhosts(ghostScores);
    sim_.drainGridTileUpdates(gridUpdates);
    sim_.drainFruitPopups(fruitPopups);

    sf::Packet snap = buildSnapshotPacket(consumed, ghostScores, gridUpdates, fruitPopups);

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
            auto t = sim_.getLevel().getTile(x, y);
            pkt << static_cast<sf::Uint8>(static_cast<int>(t));
        }
    }
    return pkt;
}

sf::Packet GameServer::buildSnapshotPacket(const std::vector<Simulation::ConsumedPellet>&  consumed,
                                           const std::vector<Simulation::EatenGhostEvent>& ghostScores,
                                           const std::vector<Simulation::GridTileUpdate>&  gridUpdates,
                                           const std::vector<Simulation::EatenGhostEvent>& fruitPopups) {
    auto s0 = sim_.getPlayerState(0);
    auto s1 = sim_.getPlayerState(1);

    sf::Packet pkt;
    pkt << std::string("SNAPSHOT")
        // Player 0
        << (float)s0.position.x << (float)s0.position.y << (sf::Uint16)s0.score << (sf::Uint8)(s0.powered ? 1 : 0)
        << (sf::Uint8)s0.livesLeft << (float)s0.deathTimeLeft
        << (float)s0.powerTimeLeft
        // Player 1
        << (float)s1.position.x << (float)s1.position.y << (sf::Uint16)s1.score << (sf::Uint8)(s1.powered ? 1 : 0)
        << (sf::Uint8)s1.livesLeft << (float)s1.deathTimeLeft << (float)s1.powerTimeLeft;

    // 4 ghost positions + facings
    for (int gi = 0; gi < 4; ++gi) {
        auto gpos = sim_.getGhostPosition(gi);
        pkt << (float)gpos.x << (float)gpos.y;
    }
    for (int gi = 0; gi < 4; ++gi) {
        pkt << (sf::Uint8)sim_.getGhostFacing(gi);
    }

    // Consumed pellet count + ghost score + grid paints + fruit popups
    pkt << static_cast<sf::Uint16>(consumed.size()) << static_cast<sf::Uint16>(ghostScores.size())
        << static_cast<sf::Uint16>(gridUpdates.size()) << static_cast<sf::Uint16>(fruitPopups.size());

    // Per-ghost active + frightened flags
    for (int gi = 0; gi < 4; ++gi) {
        bool active     = sim_.isGhostActive(gi);
        bool frightened = active && sim_.isGhostFrightened(gi);
        pkt << (sf::Uint8)(active ? 1 : 0) << (sf::Uint8)(frightened ? 1 : 0);
    }

    // Consumed pellet deltas
    for (auto& d : consumed) {
        pkt << static_cast<sf::Uint16>(d.x) << static_cast<sf::Uint16>(d.y)
            << static_cast<sf::Uint8>(d.item ? d.item->networkClearTag() : static_cast<std::uint8_t>(1));
    }

    // Ghost-eat score popups
    for (auto& ge : ghostScores) {
        pkt << (float)ge.x << (float)ge.y << (sf::Uint16)ge.points;
    }

    // Server-placed bonus fruit (or other) tiles this tick
    for (const auto& gu : gridUpdates) {
        pkt << gu.x << gu.y << gu.tile;
    }

    for (const auto& fe : fruitPopups) {
        pkt << (float)fe.x << (float)fe.y << (sf::Uint16)fe.points;
    }

    pkt << (sf::Uint8)(simPaused_ ? 1 : 0);
    sf::Uint8 out = 0;
    if (match_.isGameOver())
        out = 1;
    else if (match_.isLevelComplete())
        out = 2;
    pkt << out;

    return pkt;
}
