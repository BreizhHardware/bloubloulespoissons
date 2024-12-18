//
// Created by BreizhHardware on 18/12/2024.
//

#include "utility.h"

std::mutex mtx;
std::vector<ThreadInfo> threadInfos;

void checkThreads() {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& threadInfo : threadInfos) {
        if (threadInfo.isRunning) {
            std::cout << "Thread " << threadInfo.functionName << " (ID: " << threadInfo.id << ") is still running." << std::endl;
        } else {
            std::cout << "Thread " << threadInfo.functionName << " (ID: " << threadInfo.id << ") has stopped." << std::endl;
        }
    }
}

void displayFPS(SDL_Renderer* renderer, TTF_Font* font, int fps) {
    std::string fpsText = "FPS: " + std::to_string(fps);
    SDL_Color color = {0, 255, 0}; // Green
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

void displayUnifiedPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int unifiedX, int unifiedY) {
    std::string coordText = "Unified: (" + std::to_string(unifiedX) + ", " + std::to_string(unifiedY) + ")";
    SDL_Color textColor = {0, 255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, coordText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {10, 70, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void displayPlayerCount(SDL_Renderer* renderer, TTF_Font* font, int playerCount) {
    std::string playerCountText = "Player count: " + std::to_string(playerCount);
    SDL_Color color = {0, 255, 0}; // Green
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, playerCountText.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {windowWidth - textSurface->w - 10, 30, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet could not initialize! SDLNet_Error: " << SDLNet_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    Mix_AllocateChannels(16);

    // Charger la musique du menu
    // menuMusic = Mix_LoadMUS("../sounds/Menu.wav");
    // if (menuMusic == nullptr) {
    //     std::cerr << "Erreur de chargement de la musique du menu: " << Mix_GetError() << std::endl;
    //     return false;
    // }

    window = SDL_CreateWindow("BloubBloub les poissons",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface* iconSurface = IMG_Load("../img/logo.png");
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

void updateFishRange(std::vector<Fish>& school, int start, int end){
    while (game_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        {
            std::lock_guard<std::mutex> lock(mtx);
            Fish::insertionSort(school);
        }
        for (int i = start; i < end; ++i) {
            school[i].cycle(i);
        }
    }
}

void playerMovementThread(Player& player) {
    try {
        std::cout << "Starting playerMovementThread for player " << player.getPlayerId() << std::endl;
        while (game_running) {
            player.handlePlayerMovement(ENV_WIDTH, ENV_HEIGHT, windowWidth, windowHeight);
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60 FPS
        }
        std::cout << "Exiting playerMovementThread for player " << player.getPlayerId() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in playerMovementThread: " << e.what() << std::endl;
    }
}

void updateShark(Shark &shark) {
    while (game_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        shark.cycle();
    }
}

void renderScene(std::vector<Player>& players, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals,Shark& shark) {
    static Uint32 lastTime = 0;
    static int frameCount = 0;
    static int fps = 0;
    //std::cout << "renderScene for " << players.size() << " players" << std::endl;

    const Uint32 currentTime = SDL_GetTicks64();
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

    for (Fish& fish : school) {
        fish.draw(renderer);
    }

    for (auto& player : players) {
        player.draw(renderer);
    }

    if (isPlayingOnline) {
        displayNearbyPlayers(renderer, font, players[0], players_server, 500);
    }

    shark.draw(renderer);

    if (displayFPSFlag) {
        displayFPS(renderer, font, fps);
    }

    SDL_RenderPresent(renderer);
}

void handleClientMessages(Player& player) {
    std::cout << "messageThread started..." << std::endl;
    while (messageThreadRunning) {
        player.handleClientMessages();
    }
    std::cout << "messageThread ended" << std::endl;
}