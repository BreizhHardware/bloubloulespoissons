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
SDL_Texture* playerTexture = nullptr;
std::vector<Fish> school;

int windowWidth = 1600;
int windowHeight = 1000;

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

// Function to update a range of fish
void updateFishRange(std::vector<Fish>& school, int start, int end) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = start; i < end; ++i) {
            school[i].move();
        }
    }
}

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return 1;
    }

    for (int i = 0; i < 1000; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 50, 50, schoolTexture, renderer));
    }

    std::vector<std::thread> threads;
    int fishPerThread = 100;
    for (int i = 0; i < school.size(); i += fishPerThread) {
        threads.emplace_back(updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size())));
    }

    int playerX = windowWidth / 2;
    int playerY = windowHeight / 2;
    const int playerSpeed = 5;

    while (running) {
        handleEvents(playerX, playerY, playerSpeed);
        renderScene(playerX, playerY);
        SDL_Delay(10);
    }

    running = false;
    for (auto& thread : threads) {
        thread.join();
    }
    cleanup();

    return 0;
}

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur d'initialisation de SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Erreur d'initialisation de SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow("BloubBloub les poissons",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Erreur de création de la fenêtre: " << SDL_GetError() << std::endl;
        SDL_Quit();
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

    SDL_Surface* schoolSurface = IMG_Load("../img/poasson.png");
    schoolTexture = SDL_CreateTextureFromSurface(renderer, schoolSurface);
    SDL_FreeSurface(schoolSurface);

    SDL_Surface* playerSurface = IMG_Load("../img/player.png");
    playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    return true;
}

void handleEvents(int& playerX, int& playerY, const int playerSpeed) {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
            }
        }
    }

    if (keystate[SDL_SCANCODE_W] && keystate[SDL_SCANCODE_A]) {
        playerY -= playerSpeed;
        playerX -= playerSpeed;
    } else if (keystate[SDL_SCANCODE_W] && keystate[SDL_SCANCODE_D]) {
        playerY -= playerSpeed;
        playerX += playerSpeed;
    } else if (keystate[SDL_SCANCODE_S] && keystate[SDL_SCANCODE_A]) {
        playerY += playerSpeed;
        playerX -= playerSpeed;
    } else if (keystate[SDL_SCANCODE_S] && keystate[SDL_SCANCODE_D]) {
        playerY += playerSpeed;
        playerX += playerSpeed;
    } else if (keystate[SDL_SCANCODE_W]) {
        playerY -= playerSpeed;
    } else if (keystate[SDL_SCANCODE_S]) {
        playerY += playerSpeed;
    } else if (keystate[SDL_SCANCODE_A]) {
        playerX -= playerSpeed;
    } else if (keystate[SDL_SCANCODE_D]) {
        playerX += playerSpeed;
    } else if (keystate[SDL_SCANCODE_ESCAPE]) {
        running = false;
    } else if (keystate[SDL_SCANCODE_F11]) {
        Uint32 flags = SDL_GetWindowFlags(window);
        if (flags & SDL_WINDOW_FULLSCREEN) {
            SDL_SetWindowFullscreen(window, 0);
        } else {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
    }


    
    if (playerX < 0) {
        playerX = windowWidth;
    } else if (playerX > windowWidth) {
        playerX = 0;
    }

    if (playerY < 0) {
        playerY = windowHeight;
    } else if (playerY > windowHeight) {
        playerY = 0;
    }
}

void renderScene(int playerX, int playerY) {
    int offsetX = playerX;
    int offsetY = playerY;

    if (offsetX < 0) offsetX = 0;
    if (offsetY < 0) offsetY = 0;
    if (offsetX > ENV_WIDTH - windowWidth) offsetX = ENV_WIDTH - windowWidth;
    if (offsetY > ENV_HEIGHT - windowHeight) offsetY = ENV_HEIGHT - windowHeight;

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

    SDL_Rect playerRect = { playerX - offsetX, playerY - offsetY, 75, 75 };
    SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);

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