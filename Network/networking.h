#ifndef NETWORKING_H
#define NETWORKING_H

#include <SDL2/SDL_net.h>
#include <vector>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <chrono>
#include "../Utility/env.h"
#include "networking_client.h"
#include "../Entities/player.h"
#include "../Entities/shark.h"

inline IPaddress ip;
inline TCPsocket server;
inline std::vector<TCPsocket> clients;

bool initServer();
void acceptClients();
std::pair<int, int> updatePlayerPosition(int clientId, const std::string& direction);
void createNewPlayer(int clientId);
void updateKeepAlive(int clientId);
void checkClientAlive();
void closeServer();
void handleServerMessages();
void sendSharkPosition(TCPsocket socket, int sharkId, int x, int y);
std::string processReceivedData(const std::string& data);

#endif //NETWORKING_H