#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "fish.h"
#include "decors.h"

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

int main(int argc, char* argv[]) {
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur d'initialisation de SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialiser SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Erreur d'initialisation de SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
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

    // Créer un renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Erreur de création du renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Charger l'image de la texture pour les poissons
    SDL_Surface* fishSurface = IMG_Load("../img/mory.png");
    if (fishSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* fishTexture = SDL_CreateTextureFromSurface(renderer, fishSurface);
    SDL_FreeSurface(fishSurface);
    if (fishTexture == nullptr) {
        std::cerr << "Erreur de création de la texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<Fish> fishes;
    for (int i = 0; i < 90; ++i) {
        fishes.emplace_back(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, rand() % 3 - 1, rand() % 3 - 1, 10, 5, renderer, nullptr);
    }
    for (int i = 0; i < 10; ++i) {
        fishes.emplace_back(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, rand() % 3 - 1, rand() % 3 - 1, 10, 5, renderer, "../img/mory.png");
    }

    std::thread fishThread(updateFish, std::ref(fishes));

    Rock rock(100, 100, 50, 255, 0, 0);
    Reef reef(300, 300);
    Kelp kelp(500, 500, 100, 4, 87, 0);

    bool running = true;
    SDL_Event event;

    int playerX = WINDOW_WIDTH / 2;
    int playerY = WINDOW_HEIGHT / 2;
    const int playerSpeed = 5;

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

        rock.draw(renderer);
        reef.draw(renderer);
        kelp.draw(renderer);

        // Dessiner le joueur
        SDL_Rect playerRect = { playerX - offsetX, playerY - offsetY, 20, 20 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
        SDL_RenderFillRect(renderer, &playerRect);

        SDL_RenderPresent(renderer);
    }

    fishThread.join();
    SDL_DestroyTexture(fishTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}