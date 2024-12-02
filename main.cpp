#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdlib>
#include <algorithm>

#include "fish.h"
#include "decors.h"
#include "camera.h"
#include "env.h"
#include "player.h"

std::mutex mtx;
std::atomic<bool> running(true);

SDL_Texture* playerTexture = nullptr;
SDL_Texture* fishTextures[100]; // Adjust the size as needed
std::vector<Fish> school;

bool initSDL();
void handleQuit();
void renderScene(Player player, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals);
void cleanup();


void displayFPS(SDL_Renderer* renderer, TTF_Font* font, int fps) {
    std::string fpsText = "FPS: " + std::to_string(fps);
    SDL_Color color = {0, 255, 0}; // Vert
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, fpsText.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {windowWidth - textSurface->w - 10, 10, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int playerX, int playerY) {
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    std::string coordText = "Camera: (" + std::to_string(cameraX) + ", " + std::to_string(cameraY) + ")";
    std::string coordText2 = "Player: (" + std::to_string(playerX) + ", " + std::to_string(playerY) + ")";
    SDL_Color textColor = {0, 255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, coordText.c_str(), textColor);
    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, coordText2.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);

    SDL_Rect textRect = {10, 30, textSurface->w, textSurface->h};
    SDL_Rect textRect2 = {10, 50, textSurface2->w, textSurface2->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_RenderCopy(renderer, textTexture2, nullptr, &textRect2);

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(textSurface2);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
}

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("BloubBloub les poissons",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface* iconSurface = IMG_Load("../img/mory.png");
    if(iconSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'icône: " << IMG_GetError() << std::endl;
    } else {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Erreur de création du renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!initEnvironment(renderer)) {
        return false;
    }

    for (int i = 0; i < fishCount; ++i) {
        std::string fishTexturePath = "../img/fish/fish" + std::to_string(i) + ".png";
        SDL_Surface* fishSurface = IMG_Load(fishTexturePath.c_str());
        if (fishSurface == nullptr) {
            std::cerr << "Erreur de chargement de l'image du poisson: " << IMG_GetError() << std::endl;
            return false;
        }
        fishTextures[i] = SDL_CreateTextureFromSurface(renderer, fishSurface);
        if (fishTextures[i] == nullptr) {
            std::cerr << "Erreur de création de la texture du poisson: " << SDL_GetError() << std::endl;
            return false;
        }
        SDL_FreeSurface(fishSurface);
    }

    texturesVector = initTexture(renderer);
    return true;
}

void playerMovementThread(Player& player) {
    std::cout << "starting playerMovementThread..." << std::endl;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        player.handlePlayerMovement(ENV_WIDTH, ENV_HEIGHT, windowWidth, windowHeight);
    }
    std::cout << "playerMovementThread ended" << std::endl;
}


void fishMovementThread(std::vector<Fish>& school) {
    std::cout << "starting fishMovementThread..." << std::endl;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::sort(school.begin(), school.end(), [](const Fish &a, const Fish &b){return a.getX() < b.getX();});
        for (int i = 0; i < school.size(); ++i) {
            school[i].cycle();
        }
    }
    std::cout << "fishMovementThread ended" << std::endl;
}


int main(int argc, char* args[]) {
    if (!initSDL()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    std::vector<Kelp> kelps;
    std::vector<Rock> rocks;
    std::vector<Coral> corals;
    generateProceduralDecorations(kelps, rocks, corals,ENV_HEIGHT, ENV_WIDTH, renderer);

    for (int i = 0; i < FISH_NUMBER ; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 75, 75, renderer, rand() % 2 == 0 ? 1 : 0, fishTextures[rand() % fishCount]));
    }

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    Player player = Player(windowWidth / 2, windowHeight / 2, 5, renderer);

    std::thread player_thread(playerMovementThread, std::ref(player));
    std::thread fish_thread(fishMovementThread, std::ref(school));

    while (running) {
        renderScene(player, kelps, rocks, corals);
        handleQuit();
        SDL_Delay(10);
    }
    running = false;
    player_thread.join();
    fish_thread.join();
    /*
    for (auto& thread : threads) {
        thread.join();
    }
    */
    cleanup();
    return 0;
}

void handleQuit() {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }

    if (keystate[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }
}


void renderScene(Player player, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals) {
    static Uint32 lastTime = 0;
    static int frameCount = 0;
    static int fps = 0;

    Uint32 currentTime = SDL_GetTicks();
    frameCount++;
    if (currentTime - lastTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        lastTime = currentTime;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Camera& camera = Camera::getInstance();
    SDL_Rect backgroundRect = { -camera.getX(), -camera.getY(), ENV_WIDTH, ENV_HEIGHT };
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);

    for (const auto& kelp : kelps) {
        kelp.draw(renderer);
    }

    for (const auto& rock : rocks) {
        rock.draw(renderer);
    }

    for (const auto& coral : corals) {
        coral.draw(renderer);
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (Fish& fish : school) {
        fish.draw(renderer);
    }

    player.draw(renderer);

    displayFPS(renderer, font, fps);
    auto [playerX, playerY] = player.getPlayerPos();
    displayPlayerCoord(renderer, font, playerX, playerY);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(backgroundTexture);
    for (int i = 0; i < fishCount; ++i) {
        SDL_DestroyTexture(fishTextures[i]);
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}