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
#include "../env.h"
#include "../player.h"

class Player;

inline TCPsocket client;

bool initClient(IPaddress& ip, const char* host, int port);
void closeClient();
void sendMessage(TCPsocket socket, const std::string& message);
std::string receiveMessage(TCPsocket socket);
void handleClientMessage(Player& player);
void sendKeepAlive(int clientId);
void startKeepAlive(int clientId);


#endif //NETWORKING_CLIENT_H