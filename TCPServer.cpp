//
// Created by BreizhHardware on 12/11/2024.
//
#include "TCPServer.h"

TCPServer::TCPServer(int port, SDL_Renderer* renderer) : port(port), renderer(renderer) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void TCPServer::start() {
    serverThread = std::thread(&TCPServer::run, this);
}

void TCPServer::stop() {
    running = false;
    std::cout << "Stopping server..." << std::endl;
    std::cout << serverThread.joinable() << std::endl;
    if (serverThread.joinable()) {
        serverThread.detach();
    }
#ifdef _WIN32
    WSACleanup();
#endif
    std::cout << "Server stopped" << std::endl;
}

TCPServer::~TCPServer() {
    stop();
}

void TCPServer::run() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket" << std::endl;
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding server socket" << std::endl;
        return;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on server socket" << std::endl;
        return;
    }

    while (running) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }
        std::thread(&TCPServer::handleClient, this, clientSocket).detach();
    }

#ifdef _WIN32
    closesocket(serverSocket);
#else
    close(serverSocket);
#endif
}

void TCPServer::handleClient(int clientSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "New client connected: " << clientIP << std::endl;

    int x, y, speed;
    recv(clientSocket, (char*)&x, sizeof(x), 0);
    recv(clientSocket, (char*)&y, sizeof(y), 0);
    recv(clientSocket, (char*)&speed, sizeof(speed), 0);

    Player newPlayer(x, y, speed, renderer);
    {
        std::lock_guard<std::mutex> lock(playerMutex);
        players.push_back(newPlayer);
    }

    while (running) {
        // Handle client messages
    }

    {
        std::lock_guard<std::mutex> lock(playerMutex);
        players.erase(std::remove_if(players.begin(), players.end(), [&](const Player& p) {
            return p.getPlayerPos() == std::make_tuple(x, y);
        }), players.end());
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}