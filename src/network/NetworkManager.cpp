#include "NetworkManager.h"

NetworkManager &NetworkManager::getInstance()
{
    static NetworkManager instance;
    return instance;
}

bool NetworkManager::initializeServer(unsigned short port)
{
    isServer_ = true;

    // Start listening for connections
    if (listener_.listen(port) != sf::Socket::Done)
    {
        return false;
    }

    // Set listener to non-blocking mode
    listener_.setBlocking(false);
    return true;
}

bool NetworkManager::initializeClient(const std::string &address, unsigned short port)
{
    isServer_ = false;

    // Connect to server
    if (socket_.connect(address, port) != sf::Socket::Done)
    {
        return false;
    }

    // Set socket to non-blocking mode
    socket_.setBlocking(false);
    return true;
}

void NetworkManager::update(float deltaTime)
{
    if (isServer_)
    {
        // Accept new connections
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
        if (listener_.accept(*client) == sf::Socket::Done)
        {
            client->setBlocking(false);
            int clientId = static_cast<int>(clients_.size()) + 1;
            clients_[clientId] = std::move(client);
        }

        // Process messages from clients
        for (auto &[clientId, socket] : clients_)
        {
            sf::Packet packet;
            if (socket->receive(packet) == sf::Socket::Done)
            {
                // Broadcast to other clients
                for (auto &[otherId, otherSocket] : clients_)
                {
                    if (otherId != clientId)
                    {
                        otherSocket->send(packet);
                    }
                }
            }
        }
    }
    else
    {
        // Process messages from server
        sf::Packet packet;
        if (socket_.receive(packet) == sf::Socket::Done)
        {
            messageQueue_.push(packet);
        }
    }
}

void NetworkManager::sendPlayerState(int playerId, float x, float y, int score)
{
    sf::Packet packet;
    packet << playerId << x << y << score;

    if (isServer_)
    {
        // Server broadcasts to all clients
        for (auto &[_, socket] : clients_)
        {
            socket->send(packet);
        }
    }
    else
    {
        // Client sends to server
        socket_.send(packet);
    }
}