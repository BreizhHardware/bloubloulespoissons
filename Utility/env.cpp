#include "env.h"
#include "../Entities/player.h"
#include "../Entities/fish.h"

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
int FISH_NUMBER = 2500;
std::vector<SDL_Texture*> texturesVector;
std::atomic<bool> running(true);
std::atomic<bool> game_running(false);
std::atomic<bool> isPlayingOnline(false);
std::atomic<bool> messageThreadRunning(false);
std::atomic<bool> isHost(false);
EventHandler eventHandler;
std::atomic<bool> soundMuted(false);
SDL_Texture* playerTexture = nullptr;
SDL_Texture* fishTextures[100];
std::vector<Fish> school;
MusicManager musicManager;
std::atomic<bool> displayFPSFlag(true);


void resetAll(){
    game_running = false;
    isPlayingOnline = false;
    messageThreadRunning = false;
    isHost = false;
    players.clear();
    players_server.clear();
}


bool initEnvironment(SDL_Renderer* renderer) {
    SDL_Surface* backgroundSurface = IMG_Load("../img/background.png");
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

std::vector<SDL_Texture*> initTexture(SDL_Renderer* renderer) {
    std::vector<SDL_Texture*> textures;
    //Load the Kelp texture
    SDL_Surface* kelpSurface = IMG_Load("../img/kelp.png");
    if (kelpSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image du Kelp: " << IMG_GetError() << std::endl;
    }
    textures.push_back(SDL_CreateTextureFromSurface(renderer, kelpSurface));
    SDL_FreeSurface(kelpSurface);

    //Load the rock texture
    SDL_Surface* rockSurface = IMG_Load("../img/rock.png");
    if (rockSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image du Rock: " << IMG_GetError() << std::endl;
    }
    textures.push_back(SDL_CreateTextureFromSurface(renderer, rockSurface));
    SDL_FreeSurface(rockSurface);

    //Load the coral texture
    SDL_Surface* coralSurface = IMG_Load("../img/coral.png");
    if (coralSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image du Coral: " << IMG_GetError() << std::endl;
    }
    textures.push_back(SDL_CreateTextureFromSurface(renderer, coralSurface));
    return textures;
}