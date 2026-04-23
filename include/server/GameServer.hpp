#pragma once

#include <SFML/Network.hpp>
#include <array>
#include <chrono>
#include <memory>

#include "shared/Match.hpp"
#include "shared/Simulation.hpp"

// Encapsulates the authoritative game server:
//   - Accepts up to 2 TCP clients
//   - Drives Simulation at a fixed tick rate; Match tracks game phase (playing / over / level clear)
//   - Broadcasts SNAPSHOT packets each tick
//   - Sends the full LEVEL packet on connect
class GameServer {
   public:
    explicit GameServer(unsigned short port = 54000, int tickHz = 60);

    // Blocking main loop; match end pauses the sim until clients request RESTART.
    void run();

   private:
    void acceptConnections();
    void processClientInputs();
    void tick();
    void resetMatch();
    void broadcastLevelToAll();

    sf::Packet buildLevelPacket() const;
    sf::Packet buildSnapshotPacket(const std::vector<Simulation::ConsumedPellet>&  consumed,
                                   const std::vector<Simulation::EatenGhostEvent>& ghostScores);

    unsigned short port_;
    int            tickHz_;
    float          scale_{1.f};
    float          scaledTile_{64.f};

    sf::TcpListener                               listener_;
    std::array<std::unique_ptr<sf::TcpSocket>, 2> clients_;
    std::array<bool, 2>                           connected_{false, false};
    sf::SocketSelector                            selector_;

    Simulation                            sim_;
    Match                                 match_;
    bool                                  simPaused_{false};
    std::chrono::steady_clock::time_point lastTick_;
    std::chrono::milliseconds             stepDuration_;
};
