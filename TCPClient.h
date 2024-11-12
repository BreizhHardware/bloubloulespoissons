//
// Created by BreizhHardware on 12/11/2024.
//
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <thread>
#include <atomic>
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

class TCPClient {
public:
    TCPClient(const std::string& serverIP, int serverPort);
    void start();
    void stop();
    ~TCPClient();

private:
    std::string serverIP;
    int serverPort;
    std::atomic<bool> running;
    std::thread clientThread;

    void run();
};

#endif // TCPCLIENT_H