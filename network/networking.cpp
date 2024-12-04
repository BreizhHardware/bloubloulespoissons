//
// Created by BreizhHardware on 04/12/2024.
//

#include "networking.h"
std::unordered_map<int, std::pair<int, int>> playerPositions;

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
            clients.push_back(clientSocket);
            std::thread clientThread([clientSocket]() {
                while (running) {
                    std::string message = receiveMessage(clientSocket);
                    if (!message.empty()) {
                        std::cout << "Server received: " << message << std::endl;
                        if (message.find(";move;") != std::string::npos) {
                            int clientId;
                            char direction[10];
                            sscanf(message.c_str(), "%d;move;%s", &clientId, direction);

                            // Update player position logic
                            int newX = 0, newY = 0, newXCam = 0, newYCam = 0;
                            // Assuming you have a function to get and update player position
                            std::tie(newX, newY) = updatePlayerPosition(clientId, direction);
                            std::tie(newXCam, newYCam) = updateCameraPosition(clientId, newX, newY);

                            // Broadcast the new position to all clients
                            std::string updatedMessage = std::to_string(clientId) + ";moved;" + std::to_string(newX) + "," + std::to_string(newY) + ";" + std::to_string(newXCam) + "," + std::to_string(newYCam);
                            for (TCPsocket client : clients) {
                                sendMessage(client, updatedMessage);
                            }
                        }
                    }
                    SDL_Delay(1);
                }
                SDLNet_TCP_Close(clientSocket);
            });
            clientThread.detach();
        }
        SDL_Delay(100);
    }
}

std::pair<int, int> updatePlayerPosition(int clientId, const std::string& direction) {
    auto& pos = playerPositions[clientId];
    int& newX = pos.first;
    int& newY = pos.second;

    if (direction == "up") {
        newY -= 5;
    } else if (direction == "down") {
        newY += 5;
    } else if (direction == "left") {
        newX -= 5;
    } else if (direction == "right") {
        newX += 5;
    }
    return {newX, newY};
}

std::pair<int, int> updateCameraPosition(int clientId, int newX, int newY) {
    // Implement the logic to update the camera position based on the new player position
    int newXCam = newX, newYCam = newY;
    // Example logic (you need to replace this with your actual logic)
    return {newXCam, newYCam};
}