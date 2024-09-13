#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>

#include "fish.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ENV_WIDTH = 1600;
const int ENV_HEIGHT = 1200;

std::mutex mtx;

// Fonction pour dessiner un fond en dégradé
void drawGradientBackground(SDL_Renderer* renderer, int offsetX, int offsetY) {
    for (int y = 0; y < ENV_HEIGHT; y++) {
        Uint8 blue = static_cast<Uint8>(255 * (1.0 - static_cast<float>(y) / ENV_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 0, 0, blue, 255);
        SDL_RenderDrawLine(renderer, -offsetX, y - offsetY, ENV_WIDTH - offsetX, y - offsetY);
    }
}

// Fonction pour mettre à jour la position des poissons
void updateFish(std::vector<Fish>& fishes) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& fish : fishes) {
            fish.move();
        }
    }
}

// Fonction pour dessiner un chiffre
void drawDigit(SDL_Renderer* renderer, int digit, int x, int y) {
    const int size = 10;
    SDL_Rect segments[7] = {
        {x, y, size, 2}, // Top
        {x + size - 2, y, 2, size}, // Top-right
        {x + size - 2, y + size, 2, size}, // Bottom-right
        {x, y + 2 * size - 2, size, 2}, // Bottom
        {x, y + size, 2, size}, // Bottom-left
        {x, y, 2, size}, // Top-left
        {x, y + size - 1, size, 2} // Middle
    };

    bool on[10][7] = {
        {true, true, true, true, true, true, false}, // 0
        {false, true, true, false, false, false, false}, // 1
        {true, true, false, true, true, false, true}, // 2
        {true, true, true, true, false, false, true}, // 3
        {false, true, true, false, false, true, true}, // 4
        {true, false, true, true, false, true, true}, // 5
        {true, false, true, true, true, true, true}, // 6
        {true, true, true, false, false, false, false}, // 7
        {true, true, true, true, true, true, true}, // 8
        {true, true, true, true, false, true, true} // 9
    };

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
    for (int i = 0; i < 7; ++i) {
        if (on[digit][i]) {
            SDL_RenderFillRect(renderer, &segments[i]);
        }
    }
}

// Fonction pour afficher les FPS
void displayFPS(SDL_Renderer* renderer, int fps) {
    int x = WINDOW_WIDTH - 50;
    int y = 10;
    if (fps < 10) {
        drawDigit(renderer, fps, x, y);
    } else if (fps < 100) {
        drawDigit(renderer, fps / 10, x, y);
        drawDigit(renderer, fps % 10, x + 15, y);
    } else {
        drawDigit(renderer, fps / 100, x, y);
        drawDigit(renderer, (fps / 10) % 10, x + 15, y);
        drawDigit(renderer, fps % 10, x + 30, y);
    }
}

int main(int argc, char* argv[]) {
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur d'initialisation de SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Créer une fenêtre
    SDL_Window* window = SDL_CreateWindow("BloubBloub les poissons",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Erreur de création de la fenêtre: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Créer un renderer avec SDL_RENDERER_SOFTWARE
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Erreur de création du renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<Fish> fishes;
    for (int i = 0; i < 10; ++i) {
        fishes.emplace_back(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, rand() % 3 - 1, rand() % 3 - 1, 10, 5); // Largeur 10, Hauteur 5
    }

    std::thread fishThread(updateFish, std::ref(fishes));

    bool running = true;
    SDL_Event event;

    int playerX = WINDOW_WIDTH / 2;
    int playerY = WINDOW_HEIGHT / 2;
    const int playerSpeed = 5;

    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;
    int fps = 0;

    while (running) {
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

        // Calculer les offsets du viewport
        int offsetX = playerX - WINDOW_WIDTH / 2;
        int offsetY = playerY - WINDOW_HEIGHT / 2;

        // Empêcher le viewport de dépasser les bords de l'environnement
        if (offsetX < 0) offsetX = 0;
        if (offsetY < 0) offsetY = 0;
        if (offsetX > ENV_WIDTH - WINDOW_WIDTH) offsetX = ENV_WIDTH - WINDOW_WIDTH;
        if (offsetY > ENV_HEIGHT - WINDOW_HEIGHT) offsetY = ENV_HEIGHT - WINDOW_HEIGHT;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawGradientBackground(renderer, offsetX, offsetY);

        std::lock_guard<std::mutex> lock(mtx);
        for (auto& fish : fishes) {
            fish.draw(renderer);
        }

        // Dessiner le joueur
        SDL_Rect playerRect = { playerX - offsetX, playerY - offsetY, 20, 20 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
        SDL_RenderFillRect(renderer, &playerRect);

        // Calculer et afficher les FPS
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime >= 1000) {
            fps = frameCount;
            frameCount = 0;
            startTime = currentTime;
        }
        displayFPS(renderer, fps);

        SDL_RenderPresent(renderer);
    }

    fishThread.join();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}