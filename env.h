#ifndef ENV_H
#define ENV_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
extern TTF_Font* font;
extern int fishCount;
extern int FISH_NUMBER;

bool initEnvironment(SDL_Renderer* renderer);

#endif // ENV_H