//
// Created by BreizhHardware on 18/12/2024.
//

#ifndef CLOSE_H
#define CLOSE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "env.h"
#include "../network/networking.h"

void cleanup();
void handleQuit();
void handleQuitThread();

#endif //CLOSE_H
