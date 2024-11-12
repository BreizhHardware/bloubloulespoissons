//
// Created by BreizhHardware on 12/11/2024.
//

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include "env.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

#include "player.h"

class TCPServer {
public:
    TCPServer(int port, SDL_Renderer* renderer);
    void start();
    void stop();
    ~TCPServer();

private:
    int port;
    SDL_Renderer* renderer;
    std::thread serverThread;
    std::vector<Player> players;
    std::mutex playerMutex;

    void run();
    void handleClient(int clientSocket);
};

#endif // TCPSERVER_H