//
// Created by BreizhHardware on 12/11/2024.
//
#include "TCPClient.h"

TCPClient::TCPClient(const std::string& serverIP, int serverPort)
    : serverIP(serverIP), serverPort(serverPort), running(true) {
    std::cout << "TCPClient created" << std::endl;
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void TCPClient::start() {
    std::cout << "Starting client..." << std::endl;
    clientThread = std::thread(&TCPClient::run, this);
    std::cout << "Client started" << std::endl;
}

void TCPClient::stop() {
    running = false;
    if (clientThread.joinable()) {
        clientThread.join();
    }
#ifdef _WIN32
    WSACleanup();
#endif
}

TCPClient::~TCPClient() {
    stop();
}

void TCPClient::run() {
    std::cout << "Client running" << std::endl;
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating client socket" << std::endl;
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
#ifdef _WIN32
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
#else
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
#endif

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        return;
    }

    while (running) {
        // Communication with the server
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}