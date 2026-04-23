#include <algorithm>
#include <iostream>
#include <string>

#include "server/GameServer.hpp"

int main(int argc, char** argv) {
    unsigned short port   = 54000;
    int            tickHz = 60;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "--port" || a == "-p") && i + 1 < argc)
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        else if ((a == "--tick" || a == "-t") && i + 1 < argc)
            tickHz = std::max(1, std::stoi(argv[++i]));
    }

    GameServer server(port, tickHz);
    server.run();
    return 0;
}
