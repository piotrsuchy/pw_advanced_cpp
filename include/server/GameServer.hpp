#pragma once

#include <SFML/Network.hpp>
#include <array>
#include <chrono>
#include <memory>

#include "shared/Match.hpp"
#include "shared/Simulation.hpp"

/**
 * @brief Authoritative multiplayer server for the Pac-Man match.
 *
 * The server accepts clients, waits for readiness, advances the simulation at
 * a fixed tick rate, and broadcasts snapshots and level state.
 */
class GameServer {
   public:
    /**
     * @brief Creates a server bound to a TCP port and tick rate.
     *
     * @param port TCP port to listen on.
     * @param tickHz Fixed simulation tick rate in Hertz.
     */
    explicit GameServer(unsigned short port = 54000, int tickHz = 60);

    /**
     * @brief Runs the blocking server loop.
     *
     * The simulation remains paused after match end until clients request a
     * restart.
     */
    void run();

   private:
    void acceptConnections();
    void processClientInputs();
    void tick();
    void resetMatch();
    void broadcastLevelToAll();
    void tryStartRoundAfterReady();
    void onClientDisconnected(int slot);

    sf::Packet buildLevelPacket() const;
    sf::Packet buildSnapshotPacket(const std::vector<Simulation::ConsumedPellet>&  consumed,
                                   const std::vector<Simulation::EatenGhostEvent>& ghostScores,
                                   const std::vector<Simulation::GridTileUpdate>&  gridUpdates,
                                   const std::vector<Simulation::EatenGhostEvent>& fruitPopups);

    unsigned short port_;
    int            tickHz_;
    float          scale_{1.f};
    float          scaledTile_{64.f};

    sf::TcpListener                               listener_;
    std::array<std::unique_ptr<sf::TcpSocket>, 2> clients_;
    std::array<bool, 2>                           connected_{false, false};
    sf::SocketSelector                            selector_;

    Simulation                            sim_;
    Match                                 match_{MatchPhase::Waiting};
    std::array<bool, 2>                   playerReady_{false, false};
    bool                                  simPaused_{false};
    std::chrono::steady_clock::time_point lastTick_;
    std::chrono::milliseconds             stepDuration_;
};
