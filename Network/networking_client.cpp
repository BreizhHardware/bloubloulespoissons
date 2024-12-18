#include "networking_client.h"

extern TCPsocket server;

bool initClient(IPaddress& ip, const char* host, int port) {
    std::cout << "Initializing client..." << std::endl;
    if (SDLNet_ResolveHost(&ip, host, port) < 0) {
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        return false;
    }
    client = SDLNet_TCP_Open(&ip);
    if (!client) {
        std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return false;
    }
    std::cout << "Client initialized!" << std::endl;
    return true;
}

void closeClient() {
    try{
        if(client)
            SDLNet_TCP_Close(client);
    }catch(const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}


void sendMessage(TCPsocket socket, const std::string& message) {
    if (!socket) {
        std::cerr << "Invalid socket" << std::endl;
        return;
    }

    int len = message.length();
    int result = SDLNet_TCP_Send(socket, &len, sizeof(len));
    if (result < sizeof(len)) {
        std::cerr << "SDLNet_TCP_Send failed: " << SDLNet_GetError() << std::endl;
        //std::cerr << "Closing the game ..." << std::endl;
        //game_running = false;
        std::cout << "Return to menu..." << std::endl;
        eventHandler.triggerEvent("playerLost");
        return;
    }

    result = SDLNet_TCP_Send(socket, message.c_str(), len);
    if (result < len) {
        std::cerr << "SDLNet_TCP_Send failed: " << SDLNet_GetError() << std::endl;
    }
}

std::string receiveMessage(TCPsocket socket) {
    int len;
    if (SDLNet_TCP_Recv(socket, &len, sizeof(len)) <= 0) {
        //std::cerr << "Failed to receive message length" << std::endl;
        return "";
    }

    if (len <= 0 || len > 10000) { // Ajustez cette valeur selon vos besoins
        //std::cerr << "Invalid message length: " << len << std::endl;
        return "";
    }

    char* buffer = new(std::nothrow) char[len + 1];
    if (!buffer) {
        //std::cerr << "Memory allocation failed" << std::endl;
        return "";
    }

    if (SDLNet_TCP_Recv(socket, buffer, len) <= 0) {
        //std::cerr << "Failed to receive message" << std::endl;
        delete[] buffer;
        return "";
    }

    buffer[len] = '\0';
    std::string message(buffer);
    delete[] buffer;
    return message;
}

void handleClientMessage(Player& player) {
    std::cout << "handleClientMessage called" << std::endl;
    char message[1024];
    while (messageThreadRunning) {
        int len = SDLNet_TCP_Recv(server, message, 1024);
        if (len > 0) {
            message[len] = '\0';
            std::string msg(message);
            std::istringstream iss(msg);
            std::string token;
            while (std::getline(iss, token, ';')) {
                std::cout << "Token: " << token << std::endl; // Ajout de débogage
                if (token == "playerList") {
                    while (std::getline(iss, token, ';')) {
                        int playerId, x, y;
                        sscanf(token.c_str(), "%d,%d,%d", &playerId, &x, &y);
                        if (playerId > 0 && playerId < players.size()) {
                            players[playerId].updatePosition(x, y);
                        } else {
                            players.emplace_back(Player(x, y, 5, renderer, playerId));
                        }
                    }
                } else if (token == "newPlayer") {
                    int playerId, x, y;
                    sscanf(token.c_str(), "%d,%d,%d", &playerId, &x, &y);
                    if (playerId > 0) {
                        players.emplace_back(Player(x, y, 5, renderer, playerId));
                    }
                } else {
                    int playerId = std::stoi(token);
                    std::getline(iss, token, ';');
                    int unifiedX = std::stoi(token);
                    std::getline(iss, token, ';');
                    int unifiedY = std::stoi(token);
                    std::getline(iss, token, ';');
                    int PlayerX = std::stoi(token);
                    std::getline(iss, token, ';');
                    int PlayerY = std::stoi(token);
                    std::cout << "Unified position: " << unifiedX << ", " << unifiedY << std::endl;
                    if (playerId > 0 && playerId < players.size()) {
                        players[playerId].updatePosition(PlayerX, PlayerY);
                    }
                }
            }
        }
    }
}

void sendKeepAlive(TCPsocket serverSocket) {
    while (game_running) {
        std::string keepAliveMessage = "keepalive";
        sendMessage(serverSocket, keepAliveMessage);
        SDL_Delay(3000); // Envoyer un message de keepalive toutes les 3 secondes
    }
}

void startKeepAlive(TCPsocket serverSocket) {
    std::thread([serverSocket] () {
        while (messageThreadRunning) {
            sendKeepAlive(serverSocket);
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }).detach();
}
