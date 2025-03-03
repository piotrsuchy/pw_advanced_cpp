#ifndef PACMAN_NETWORK_MANAGER_H
#define PACMAN_NETWORK_MANAGER_H

#include <SFML/Network.hpp>
#include <memory>
#include <map>
#include <queue>
#include <string>

/**
 * @brief Class managing network communication for multiplayer gameplay
 *
 * This class handles all network-related functionality including:
 * - Player connections
 * - Game state synchronization
 * - Message passing between clients and server
 */
class NetworkManager
{
public:
    /**
     * @brief Singleton instance getter
     * @return Reference to the NetworkManager instance
     */
    static NetworkManager &getInstance();

    /**
     * @brief Initialize as server
     * @param port Port to listen on
     * @return true if successful, false otherwise
     */
    bool initializeServer(unsigned short port);

    /**
     * @brief Initialize as client
     * @param address Server address
     * @param port Server port
     * @return true if successful, false otherwise
     */
    bool initializeClient(const std::string &address, unsigned short port);

    /**
     * @brief Update network state
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);

    /**
     * @brief Send player state to other clients/server
     * @param playerId Player identifier
     * @param x X position
     * @param y Y position
     * @param score Current score
     */
    void sendPlayerState(int playerId, float x, float y, int score);

private:
    NetworkManager() = default; // Private constructor for singleton
    ~NetworkManager() = default;

    NetworkManager(const NetworkManager &) = delete;
    NetworkManager &operator=(const NetworkManager &) = delete;

    bool isServer_;                                         // Whether this instance is a server
    sf::TcpListener listener_;                              // Server listener
    sf::TcpSocket socket_;                                  // Client socket
    std::map<int, std::unique_ptr<sf::TcpSocket>> clients_; // Connected clients
    std::queue<sf::Packet> messageQueue_;                   // Message queue
};

#endif // PACMAN_NETWORK_MANAGER_H