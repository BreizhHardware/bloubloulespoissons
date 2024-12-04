//
// Created by BreizhHardware on 04/12/2024.
//

#include "networking.h"

bool initServer() {
    std::cout << "Initializing server..." << std::endl;
    if (SDLNet_ResolveHost(&ip, NULL, 1234) == -1) {
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        return false;
    }

    server = SDLNet_TCP_Open(&ip);
    if (!server) {
        std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return false;
    }
    std::cout << "Server initialized!" << std::endl;
    return true;
}

void acceptClients() {
    while (running) {
        TCPsocket clientSocket = SDLNet_TCP_Accept(server);
        if (clientSocket) {
            std::thread clientThread([clientSocket]() {
                while (running) {
                    std::string message = receiveMessage(clientSocket);
                    if (!message.empty()) {
                        std::cout << "Server received: " << message << std::endl;
                        sendMessage(clientSocket, "Echo: " + message); // Echo the message back to the client
                    }
                    SDL_Delay(100);
                }
                SDLNet_TCP_Close(clientSocket);
            });
            clientThread.detach();
        }
        SDL_Delay(100);
    }
}