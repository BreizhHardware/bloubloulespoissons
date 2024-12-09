//
// Created by BreizhHardware on 04/12/2024.
//

#include "networking.h"
std::unordered_map<int, std::pair<int, int>> playerPositions;
std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> lastKeepAlive;

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
            int clientId = clients.size() - 1;
            createNewPlayer(clientId);
            updateKeepAlive(clientId);
            startKeepAlive(clientId);
            std::thread clientThread([clientSocket, clientId]() {
                while (running) {
                    std::string message = receiveMessage(clientSocket);
                    if (!message.empty()) {
                        std::cout << "Server received: " << message << std::endl;
                        if (message.find(";move;") != std::string::npos) {
                            char direction[20];
                            sscanf(message.c_str(), "%d;move;%s", &clientId, &direction);

                            int newX = 0, newY = 0, newUnifedX = 0, newUnifedY = 0;
                            std::cout << "Client " << clientId << " moved " << direction << std::endl;
                            std::tie(newX, newY) = updatePlayerPosition(clientId, direction);
                            std::string updatedMessage = std::to_string(clientId) + ";moved;" + std::to_string(newX) + "," + std::to_string(newY);
                            for (TCPsocket client : clients) {
                                sendMessage(client, updatedMessage);
                            }
                        }
                    }
                    SDL_Delay(1);
                }
                lastKeepAlive.erase(clientId);
                SDLNet_TCP_Close(clientSocket);
            });
            clientThread.detach();
        }
        SDL_Delay(100);
    }
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string temp;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!temp.empty()) { // On évite d'ajouter des chaînes vides si nécessaire
                result.push_back(temp);
            }
            temp.clear(); // Réinitialise pour le prochain segment
        } else {
            temp += ch;
        }
    }
    if (!temp.empty()) { // Ajouter le dernier segment s'il existe
        result.push_back(temp);
    }
    return result;
}

std::pair<int, int> updatePlayerPosition(int clientId, const std::string& direction) {
    std::cout << "Updating player position for client " << clientId << " Direction: "<< direction << std::endl;
    auto& pos = playerPositions[clientId];
    int& newX = pos.first;
    int& newY = pos.second;
    int speed = 5;

    std::vector<std::string> directions = split(direction, '-');

    if(directions.size() > 1 ){
        speed = 3;
    }

    for(const std::string dist : directions){
        if (dist == "up") {
            if (newY-speed < 0) {
                newY = 0;
            } else {
                newY -= speed;
            }
        } else if (dist == "down") {
            if (newY+speed > ENV_HEIGHT - windowHeight) {
                newY = ENV_HEIGHT - windowHeight;
            } else {
                newY += speed;
            }
        } else if (dist == "left") {
            if (newX-speed < 0) {
                newX = 0;
            } else {
                newX -= speed;
            }
        } else if (dist == "right") {
            if (newX+speed > ENV_WIDTH - windowWidth) {
                newX = ENV_WIDTH - windowWidth;
            } else {
                newX += speed;
            }
        }
    }

    return {newX, newY};
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

void updateKeepAlive(int clientId) {
    lastKeepAlive[clientId] = std::chrono::steady_clock::now();
}

void checkClientAlive() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = lastKeepAlive.begin(); it != lastKeepAlive.end();) {
        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > 5) {
            std::cerr << "Client " << it->first << " is not responding. Removing..." << std::endl;
            SDLNet_TCP_Close(clients[it->first]);
            clients.erase(clients.begin() + it->first);
            it = lastKeepAlive.erase(it);
        }
        else {
            ++it;
        }
    }
}
