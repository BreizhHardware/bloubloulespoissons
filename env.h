//
// Created by BreizhHardware on 06/11/2024.
//

#ifndef ENV_H
#define ENV_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

extern int windowWidth;
extern int windowHeight;
extern int playerBaseX;
extern int playerBaseY;
extern const int MARGIN_WIDTH;
extern const int MARGIN_HEIGHT;
extern int ENV_HEIGHT;
extern int ENV_WIDTH;
extern SDL_Texture* backgroundTexture;
extern SDL_Renderer* renderer;
extern SDL_Window* window;

bool initEnvironment(SDL_Renderer* renderer);

#endif // ENV_H