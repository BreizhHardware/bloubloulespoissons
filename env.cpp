#include "env.h"

int windowWidth = 1500;
int windowHeight = 800;
int playerBaseX = windowWidth / 2;
int playerBaseY = windowHeight / 2;
const int MARGIN_WIDTH = 100;
const int MARGIN_HEIGHT = 100;
int ENV_HEIGHT = 0;
int ENV_WIDTH = 0;
SDL_Texture* backgroundTexture = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;
TTF_Font* font = nullptr;

bool initEnvironment(SDL_Renderer* renderer) {
    SDL_Surface* backgroundSurface = IMG_Load("../img/background.jpg");
    if (backgroundSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image de fond: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    ENV_WIDTH = backgroundSurface->w;
    ENV_HEIGHT = backgroundSurface->h;
    SDL_FreeSurface(backgroundSurface);

    font = TTF_OpenFont("../fonts/arial.ttf", 16);
    if (font == nullptr) {
        std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}