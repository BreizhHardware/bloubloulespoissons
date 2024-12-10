#ifndef ENV_H
#define ENV_H

#include <atomic>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Player;

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
extern std::vector<SDL_Texture*> texturesVector;
extern std::atomic<bool> running;
extern std::atomic<bool> game_running;
extern std::atomic<bool> isPlayingOnline;
extern std::atomic<bool> messageThreadRunning;
extern std::vector<Player> players;
extern std::vector<Player> players_server;
extern std::atomic<bool> isHost;

bool initEnvironment(SDL_Renderer* renderer);
std::vector<SDL_Texture*> initTexture(SDL_Renderer* renderer);

#endif // ENV_H