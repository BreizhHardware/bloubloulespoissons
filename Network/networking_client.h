//
// Created by BreizhHardware on 04/12/2024.
//

#ifndef NETWORKING_CLIENT_H
#define NETWORKING_CLIENT_H
#include <SDL2/SDL_net.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "../Utility/env.h"
#include "../player.h"

class Player;

inline TCPsocket client;

bool initClient(IPaddress& ip, const char* host, int port);
void closeClient();
void sendMessage(TCPsocket socket, const std::string& message);
std::string receiveMessage(TCPsocket socket);
void handleClientMessage(Player& player);
void sendKeepAlive(TCPsocket serverSocket);
void startKeepAlive(TCPsocket serverSocket);

#endif //NETWORKING_CLIENT_H
