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
int fishCount = 0;
int FISH_NUMBER = 1500;

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

    // Count the number of file in the ../img/fish directory
    DIR* dir;
    struct dirent* ent;
    struct stat st;
    if ((dir = opendir("../img/fish")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filePath = std::string("../img/fish/") + ent->d_name;
            if (stat(filePath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                fishCount++;
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Erreur de lecture du dossier ../img/fish" << std::endl;
        return false;
    }

    return true;
}