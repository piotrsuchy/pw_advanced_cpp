#include <SFML/Network.hpp>
#include <algorithm>
#include <iostream>
#include <string>

#include "client/GameClient.hpp"

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

    GameClient client(ip, port);
    client.run();
    return 0;
}
