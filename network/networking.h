//
// Created by BreizhHardware on 04/12/2024.
//

#ifndef NETWORKING_H
#define NETWORKING_H

#include <SDL2/SDL_net.h>
#include <vector>
#include <iostream>
#include <thread>
#include "../env.h"
#include "networking_client.h"

inline IPaddress ip;
inline TCPsocket server;
inline std::vector<TCPsocket> clients;

bool initServer();
void acceptClients();

#endif //NETWORKING_H
