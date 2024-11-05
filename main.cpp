#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "fish.h"
#include "decors.h"
#include "camera.h"

std::mutex mtx;
std::atomic<bool> running(true);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* schoolTexture = nullptr;
SDL_Texture* playerTexture = nullptr;
std::vector<Fish> school;
TTF_Font* font = nullptr;

int windowWidth = 1500;
int windowHeight = 800;
int playerBaseX = windowWidth / 2;
int playerBaseY = windowHeight / 2;

Rock rock(0, 0, 50, 255, 0, 0);
Reef reef(300, 300);
Kelp kelp(500, 500, 100, 4, 87, 0);

bool initSDL();
void handleEvents(int& playerX, int& playerY, int playerSpeed);
void renderScene(int playerX, int playerY, int *fig);
void cleanup();

void drawGradientBackground(SDL_Renderer* renderer) {
    for (int y = 0; y < ENV_HEIGHT; y++) {
        Uint8 blue = static_cast<Uint8>(255 * (1.0 - static_cast<float>(y) / ENV_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 0, 0, blue, 255);
        SDL_RenderDrawLine(renderer, 0, y, ENV_WIDTH, y);
    }
}

void drawGridBackground(SDL_Renderer* renderer) {
    // Dessiner le fond bleu
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    // Couleur des lignes de la grille
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Dessiner les lignes horizontales et les coordonnées y
    for (int y = 0; y < ENV_HEIGHT; y += 50) {
        SDL_RenderDrawLine(renderer, 0, y, ENV_WIDTH, y);
        // Afficher les coordonnées y
        std::string yText = std::to_string(y);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, yText.c_str(), {255, 255, 255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {0, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    // Dessiner les lignes verticales et les coordonnées x
    for (int x = 0; x < ENV_WIDTH; x += 50) {
        SDL_RenderDrawLine(renderer, x, 0, x, ENV_HEIGHT);
        // Afficher les coordonnées x
        std::string xText = std::to_string(x);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, xText.c_str(), {255, 255, 255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {x, 0, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

// Function to update a range of fish
void updateFishRange(std::vector<Fish>& school, int start, int end){
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = start; i < end; ++i) {
            school[i].move();
        }
    }
}

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

void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font) {
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    // Code pour afficher les coordonnées de la caméra
    std::string coordText = "Camera: (" + std::to_string(cameraX) + ", " + std::to_string(cameraY) + ")";
    std::string coordText2 = "Player: (" + std::to_string(cameraX + playerBaseX) + ", " + std::to_string(cameraY + playerBaseY) + ")";
    SDL_Color textColor = {0, 255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, coordText.c_str(), textColor);
    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, coordText2.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);

    SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
    SDL_Rect textRect2 = {10, 30, textSurface2->w, textSurface2->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_RenderCopy(renderer, textTexture2, nullptr, &textRect2);

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(textSurface2);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
}

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return 1;
    }

    for (int i = 0; i < 1000; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 50, 50, schoolTexture, renderer, rand() % 2 == 0 ? 1 : 0));
    }

    std::vector<std::thread> threads;
    int fishPerThread = 100;
    for (int i = 0; i < school.size(); i += fishPerThread) {
        threads.emplace_back(updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size())));
    }

    int playerX = windowWidth / 2;
    int playerY = windowHeight / 2;
    const int playerSpeed = 5;
    int fig = 1;
    while (running) {
        handleEvents(playerX, playerY, playerSpeed);
        renderScene(playerX, playerY, &fig);
        //std::cout << "Window size: " << windowWidth << "x" << windowHeight << std::endl;
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

    if (TTF_Init() == -1) {
        std::cerr << "Erreur d'initialisation de SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    font = TTF_OpenFont("../fonts/arial.ttf", 16);
    if (font == nullptr) {
        std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow("BloubBloub les poissons",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);
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

    SDL_Surface* playerSurface = IMG_Load("../img/player/player-full.png");
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
        }
    }

    Camera& camera = Camera::getInstance();

    if (keystate[SDL_SCANCODE_W]) {
        if(camera.getY() > -playerBaseY) {
            camera.move(0, -playerSpeed);
        }
    }
    if (keystate[SDL_SCANCODE_S]) {
        if(camera.getY() < ENV_HEIGHT - windowHeight) {
            camera.move(0, playerSpeed);
        }
    }
    if (keystate[SDL_SCANCODE_A]) {
        if(camera.getX() > -playerBaseX) {
            camera.move(-playerSpeed, 0);
        }
    }
    if (keystate[SDL_SCANCODE_D]) {
        if(camera.getX() < ENV_WIDTH - windowWidth) {
            camera.move(playerSpeed, 0);
        }
    }
    if (keystate[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }

    // Ensure player stays within environment bounds
    if (playerX < 0) {
        playerX = 0;
    } else if (playerX > ENV_WIDTH) {
        playerX = ENV_WIDTH;
    }

    if (playerY < 0) {
        playerY = 0;
    } else if (playerY > ENV_HEIGHT) {
        playerY = ENV_HEIGHT;
    }
}

void renderScene(int playerX, int playerY, int *fig) {
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

    //drawGridBackground(renderer);
    drawGradientBackground(renderer);

    rock.draw(renderer);
    reef.draw(renderer);
    kelp.draw(renderer);

    std::lock_guard<std::mutex> lock(mtx);
    for (auto& fish : school) {
        fish.draw(renderer);
    }

    // SDL_Rect playerRect = { playerX, playerY, 75, 75 };
    // SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
    SDL_Rect playerRect = {0, 0, 513, 600};
    if (*fig == 1) {
        playerRect = {46, 26, 442, 541};
        *fig = 2;
    } else if (*fig == 2) {
        playerRect = {560, 23, 426, 536};
        *fig = 3;
    }else if (*fig == 3) {
        playerRect = {986, 23, 469, 530};
        *fig = 4;
    }else if (*fig == 4) {
        playerRect = {1436, 23, 465, 520};
        *fig = 1;
    }

    SDL_Rect playerPos = {playerX, playerY, 75, 75};
    SDL_RenderCopyEx(renderer, playerTexture, &playerRect, &playerPos, 0, nullptr, SDL_FLIP_NONE);

    displayFPS(renderer, font, fps);
    displayPlayerCoord(renderer, font);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();
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