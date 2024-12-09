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
            int clientId = clients.size() - 1; // Assign a unique client ID
            createNewPlayer(clientId); // Create a new player for the new client
            std::thread clientThread([clientSocket, clientId]() {
                while (running) {
                    std::string message = receiveMessage(clientSocket);
                    if (!message.empty()) {
                        std::cout << "Server received: " << message << std::endl;
                        if (message.find(";move;") != std::string::npos) {
                            char direction[10];
                            sscanf(message.c_str(), "%d;move;%s", &clientId, direction);

                            int newX = 0, newY = 0, newXCam = 0, newYCam = 0;
                            std::tie(newX, newY) = updatePlayerPosition(clientId, direction);
                            std::tie(newXCam, newYCam) = updateCameraPosition(clientId, newX, newY);

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

void createNewPlayer(int clientId) {
    if (clientId <= 0) {
        std::cerr << "Invalid client ID: " << clientId << std::endl;
        return;
    }
    // Create a new player at a default position (e.g., 0, 0)
    Player newPlayer(0, 0, 5, renderer, clientId);
    players.push_back(newPlayer);
    playerPositions[clientId] = {0, 0}; // Initialize player position

    // Send the list of existing players to the new client
    std::string playerListMessage = "playerList;";
    for (auto& player : players) {
        auto [x, y] = player.getPlayerPos();
        playerListMessage += std::to_string(player.getPlayerId()) + "," + std::to_string(x) + "," + std::to_string(y) + ";";
    }
    sendMessage(clients[clientId], playerListMessage);

    // Broadcast the new player to all clients
    std::string newPlayerMessage = "newPlayer;" + std::to_string(clientId) + ",0,0;";
    for (TCPsocket client : clients) {
        sendMessage(client, newPlayerMessage);
    }
}