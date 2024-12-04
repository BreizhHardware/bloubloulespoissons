//
// Created by BreizhHardware on 04/12/2024.
//

#ifndef NETWORKING_CLIENT_H
#define NETWORKING_CLIENT_H
#include <SDL2/SDL_net.h>
#include <iostream>

inline TCPsocket client;

bool initClient(IPaddress& ip, const char* host, int port);
void sendMessage(TCPsocket socket, const std::string& message);
std::string receiveMessage(TCPsocket socket);


#endif //NETWORKING_CLIENT_H
