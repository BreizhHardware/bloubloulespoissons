//
// Created by BreizhHardware on 08/12/2024.
//

#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <cmath>
#include <string>
#include "player.h"

double calculateDistance(int x1, int y1, int x2, int y2);
void displayNearbyPlayers(SDL_Renderer* renderer, TTF_Font* font, Player& currentPlayer, std::vector<Player>& players, double threshold);

#endif //DISPLAY_H
