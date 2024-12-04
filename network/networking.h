//
// Created by BreizhHardware on 04/12/2024.
//

#ifndef NETWORKING_H
#define NETWORKING_H

#include <SDL2/SDL_net.h>
#include <vector>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "../env.h"
#include "networking_client.h"

inline IPaddress ip;
inline TCPsocket server;
inline std::vector<TCPsocket> clients;

bool initServer();
void acceptClients();
std::pair<int, int> updatePlayerPosition(int clientId, const std::string& direction);
std::pair<int, int> updateCameraPosition(int clientId, int newX, int newY);

#endif //NETWORKING_H
