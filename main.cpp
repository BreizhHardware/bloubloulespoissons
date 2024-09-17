#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "fish.h"
#include "decors.h"

std::mutex mtx;
std::atomic<bool> running(true);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* schoolTexture = nullptr;
std::vector<Fish> school;

Rock rock(100, 100, 50, 255, 0, 0);
Reef reef(300, 300);
Kelp kelp(500, 500, 100, 4, 87, 0);

bool initSDL();
void handleEvents(int& playerX, int& playerY, const int playerSpeed);
void renderScene(int playerX, int playerY);
void cleanup();

void drawGradientBackground(SDL_Renderer* renderer, int offsetX, int offsetY) {
    for (int y = 0; y < ENV_HEIGHT; y++) {
        Uint8 blue = static_cast<Uint8>(255 * (1.0 - static_cast<float>(y) / ENV_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 0, 0, blue, 255);
        SDL_RenderDrawLine(renderer, -offsetX, y - offsetY, ENV_WIDTH - offsetX, y - offsetY);
    }
}

// Fonction pour mettre à jour la position des poissons
void updateFish(std::vector<Fish>& school) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& fish : school) {
            fish.move();
        }
    }
}

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return 1;
    }

    for (int i = 0; i < 100; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 50, 50, schoolTexture, renderer));
    }
    std::thread fishThread(updateFish, std::ref(school));

    

    int playerX = WINDOW_WIDTH / 2;
    int playerY = WINDOW_HEIGHT / 2;
    const int playerSpeed = 5;

    while (running) {
        handleEvents(playerX, playerY, playerSpeed);
        renderScene(playerX, playerY);
        SDL_Delay(10);
    }

    running = false;
    fishThread.join();
    cleanup();

    return 0;
}

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur d'initialisation de SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialiser SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Erreur d'initialisation de SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow("BloubBloub les poissons",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Erreur de création de la fenêtre: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Erreur de création du renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    SDL_Surface* schoolSurface = IMG_Load("../img/poasson.png");
    schoolTexture = SDL_CreateTextureFromSurface(renderer, schoolSurface);
    SDL_FreeSurface(schoolSurface);

    return true;
}

void handleEvents(int& playerX, int& playerY, const int playerSpeed) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_z:
                    playerY -= playerSpeed;
                    break;
                case SDLK_s:
                    playerY += playerSpeed;
                    break;
                case SDLK_q:
                    playerX -= playerSpeed;
                    break;
                case SDLK_d:
                    playerX += playerSpeed;
                    break;
            }
        }
    }
}

void renderScene(int playerX, int playerY) {
    int offsetX = playerX - WINDOW_WIDTH / 2;
    int offsetY = playerY - WINDOW_HEIGHT / 2;

    if (offsetX < 0) offsetX = 0;
    if (offsetY < 0) offsetY = 0;
    if (offsetX > ENV_WIDTH - WINDOW_WIDTH) offsetX = ENV_WIDTH - WINDOW_WIDTH;
    if (offsetY > ENV_HEIGHT - WINDOW_HEIGHT) offsetY = ENV_HEIGHT - WINDOW_HEIGHT;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawGradientBackground(renderer, offsetX, offsetY);

    rock.draw(renderer);
    reef.draw(renderer);
    kelp.draw(renderer);

    std::lock_guard<std::mutex> lock(mtx);
    for (auto& fish : school) {
        fish.draw(renderer);
    }

    SDL_Rect playerRect = { playerX - offsetX, playerY - offsetY, 20, 20 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    SDL_DestroyTexture(schoolTexture);
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}