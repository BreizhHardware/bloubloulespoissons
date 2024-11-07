#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdlib>

#include "fish.h"
#include "decors.h"
#include "camera.h"
#include "env.h"

std::mutex mtx;
std::atomic<bool> running(true);

SDL_Texture* playerTexture = nullptr;
std::vector<Fish> school;

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
            school[i].cycle();
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

void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int playerX, int playerY) {
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    // Code pour afficher les coordonnées de la caméra
    std::string coordText = "Camera: (" + std::to_string(cameraX) + ", " + std::to_string(cameraY) + ")";
    std::string coordText2 = "Player: (" + std::to_string(playerX) + ", " + std::to_string(playerY) + ")";
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

    return true;
}

int main(int argc, char* args[]) {
    if (!initSDL()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    for (int i = 0; i < 1000; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 50, 50, renderer, rand() % 2 == 0 ? 1 : 0));
    }
    std::cout << "Thread: " << std::thread::hardware_concurrency() << std::endl;
    std::vector<std::thread> threads;
    int fishPerThread = school.size() / std::thread::hardware_concurrency();
    for (int i = 0; i < school.size(); i += fishPerThread) {
        threads.emplace_back(updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size())));
    }
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
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
        std::cout << "PlayerY: " << playerY << " Camera: " << camera.getY() << " ENV_HEIGHT: " << ENV_HEIGHT << " windowHeight: " << windowHeight << " playerBaseY: " << playerBaseY << std::endl;
        if (camera.getY() > 0 && playerY == playerBaseY) {
            camera.move(0, -playerSpeed);
        }else if (playerY > 0) {
            playerY -= playerSpeed;
        }
    }
    if (keystate[SDL_SCANCODE_S]) {
        // std::cout << "PlayerY: " << playerY << " Camera: " << camera.getY() << " ENV_HEIGHT: " << ENV_HEIGHT << " windowHeight: " << windowHeight << " playerBaseY: " << playerBaseY << std::endl;

        if ((camera.getY() < ENV_HEIGHT-windowHeight) && (playerY == playerBaseY)) {
            camera.move(0, playerSpeed);
        }else if (playerY < windowHeight-75) {
            playerY += playerSpeed;
        }
    }
    if (keystate[SDL_SCANCODE_A]) {
        if(camera.getX() > 0 && (playerX == playerBaseX)) {
            camera.move(-playerSpeed, 0);
        }else if (playerX > 0) {
            playerX -= playerSpeed;
        }
    }
    if (keystate[SDL_SCANCODE_D]) {
        if(camera.getX() < ENV_WIDTH - windowWidth && (playerX == playerBaseX)) {
            camera.move(playerSpeed, 0);
        }else if (playerX < windowWidth) {
            playerX += playerSpeed;
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
    //drawGradientBackground(renderer);
    Camera& camera = Camera::getInstance();
    SDL_Rect backgroundRect = { -camera.getX(), -camera.getY(), ENV_WIDTH, ENV_HEIGHT };
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);

    rock.draw(renderer);
    //reef.draw(renderer);
    kelp.draw(renderer);

    std::lock_guard<std::mutex> lock(mtx);
    for (auto& fish : school) {
        fish.draw(renderer);
    }

    // SDL_Rect playerRect = { playerX, playerY, 75, 75 };
    // SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
    SDL_Rect playerRect = {0, 0, 513, 600};
    if (*fig < 5 ) {
        playerRect = {46, 26, 442, 541};
    } else if (*fig < 8) {
        playerRect = {560, 23, 426, 536};
    }else if (*fig < 11) {
        playerRect = {986, 23, 469, 530};
    }else if (*fig < 14) {
        playerRect = {1436, 23, 465, 520};
    }else{
        *fig = 0;
    }
    *fig += 1;

    SDL_Rect playerPos = {playerX, playerY, 75, 75};
    SDL_RenderCopyEx(renderer, playerTexture, &playerRect, &playerPos, 0, nullptr, SDL_FLIP_NONE);

    displayFPS(renderer, font, fps);
    displayPlayerCoord(renderer, font, playerX, playerY);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(backgroundTexture);
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}