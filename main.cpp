#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdlib>
#include <algorithm>
#include <random>

#include "fish.h"
#include "decors.h"
#include "camera.h"
#include "display.h"
#include "env.h"
#include "player.h"
#include "menu.h"
#include "network/networking.h"
#include "network/networking_client.h"
#include "shark.h"


#include <system_error>

std::mutex mtx;
std::atomic<bool> menuRunning(true);

std::mutex coutMutex;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* fishTextures[100]; // Adjust the size as needed
std::vector<Fish> school;
std::vector<Player> players;
std::vector<Player> players_server;

struct ThreadInfo {
    std::thread::id id;
    std::string functionName;
};

std::vector<ThreadInfo> threadInfos;

bool initSDL();
void handleQuit();

void renderScene(std::vector<Player>& players, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals,Shark& shark );

void cleanup();
void closeGame();
void displayFPS(SDL_Renderer* renderer, TTF_Font* font, int fps);
void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int playerX, int playerY);
void displayUnifiedPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int unifiedX, int unifiedY);
void displayPlayerCount(SDL_Renderer* renderer, TTF_Font* font, int playerCount);
void playerMovementThread(Player& player);
void handleClientMessages(Player& player);
void handleQuitThread();
void HandleMenuClick(Menu& menu);
void updateFishRange(std::vector<Fish>& school, int start, int end);
int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(int argc, char* args[]);
int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(int argc, std::string args);

template <typename Function, typename... Args>
std::thread createThread(std::string key, Function&& func, Args&&... args) {
    try {
        std::cout << "Creating thread: " << key << std::endl;
        std::thread thread([key, func = std::forward<Function>(func), ...args = std::forward<Args>(args)]() mutable {
            ThreadInfo info;
            info.id = std::this_thread::get_id();
            info.functionName = key;
            {
                std::lock_guard<std::mutex> lock(mtx);
                threadInfos.push_back(info);
            }
            func(std::forward<Args>(args)...);
            std::cout << "ThreadID = " << info.id << " ThreadFunction = " << info.functionName << std::endl;
        });
        return thread;
    } catch (const std::system_error& e) {
        std::cerr << "Failed to create thread: " << e.what() << std::endl;
        throw;
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

void handleClientMessages(Player& player) {
    std::cout << "messageThread started..." << std::endl;
    while (messageThreadRunning) {
        player.handleClientMessages();
    }
    std::cout << "messageThread ended" << std::endl;
}

void handleQuitThread() {
    std::cout << "handleQuitThread..." << std::endl;
    while (game_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        handleQuit();
    }
    std::cout << "handleQuitThread" << std::endl;
};

void HandleMenuClick(Menu& menu){
    while (running) {
        menu.handleClickedButton();
    }
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

void updateShark(Shark &shark) {
    while (game_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        shark.cycle();
    }
}

int main(int argc, char* args[]) {

    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "0");

    if (!initSDL()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    Menu menu(renderer);
    menu.addPage("Main");
    menu.addPage("Multi");
    menu.addPage("Multi-Join");
    menu.changePage("Main");

    std::thread menu_thread = createThread("Menu thread", HandleMenuClick, std::ref(menu));

    menu.addText("Main", (windowWidth/2) - 300, 50, 600, 100, "BloubBloub les poissons", 1024);

    menu.addText("Multi-Join", (windowWidth/2) - 100, 50, 200, 100, "Join", 1024);

    menu.addButton("Main", (windowWidth/2) - 100, windowHeight/2 - 25, 200, 50, "Solo", 1024, [](){
        std::cout << "SOlo" << std::endl;
        menuRunning = false;
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(0, nullptr);
        
    });

    menu.addButton("Main", (windowWidth/2) - 100, (windowHeight/2 + 75) - 25, 200, 50, "Multi (WIP)", 1024, [&menu](){
        std::cout << "Multi" << std::endl;
        menu.changePage("Multi");
    });

    menu.addButton("Multi", (windowWidth/2) - 100, windowHeight/2 - 25, 200, 50, "Host", 1024, [&menu](){
        std::cout << "Host" << std::endl;
        isPlayingOnline = true;
        menuRunning = false;
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(0, "");
    });

    menu.addButton("Multi", (windowWidth/2) - 100, (windowHeight/2 + 75) - 25, 200, 50, "Join", 1024, [&menu](){
        menu.changePage("Multi-Join");
    });

    // menu.addButton("Multi", (windowWidth/2) - 100, windowHeight/2 - 25, 200, 50, "Retour", 1024, [&menu](){
    //     menu.changePage("Main");
    // });

    menu.addButton("Multi-Join", (windowWidth/2) - 100, windowHeight/2 - 25, 200, 50, "", 24, [](){
        std::cout << "IP input button clicked" << std::endl;
    }, true);

    menu.addButton("Multi-Join", (windowWidth/2) - 100, (windowHeight/2 + 75) - 25, 200, 50, "Join !", 1024, [&menu](){
        std::cout << "Join" << std::endl;
        isPlayingOnline = true;
        menuRunning = false;
        int port = 1234;
        //char* ip = "10.30.42.206";
        // Pour l'ip récupère l'interieur du bouton IP input
        std::vector<Button> buttons = menu.getButtons();
        std::string ip = buttons[0].inputText;
        std::cout << ip << std::endl;
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(port, ip);
    });

    menu.addButton("Multi-Join", (windowWidth/2) - 100, windowHeight/2 + 125, 200, 50, "Retour", 1024, [&menu](){
         menu.changePage("Multi");
    });
    menu.addButton("Multi", (windowWidth/2) - 100, windowHeight/2 + 125, 200, 50, "Retour", 1024, [&menu](){
         menu.changePage("Main");
    });

    std::thread quit_thread = createThread("Quit thread", handleQuitThread);

    if (argc > 1 && std::string(args[1]) == "69") {
        std::cout << "69" << std::endl;
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(0, nullptr);
    }else if (argc > 1 && std::string(args[1]) == "420") {
        std::cout << "420" << std::endl;
        std::thread timer_thread([](){
            std::this_thread::sleep_for(std::chrono::seconds(10));
            game_running = false;
        });
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(0, nullptr);
        timer_thread.join();
    }else if (argc > 1 && std::string(args[1]) == "80085") {
        isPlayingOnline = true;
        menuRunning = false;
        pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(0, "");
    }


    //menu.addButton((windowWidth/2) - 100, (windowHeight/2 + 25) + 50, 200, 50, "Multi", 1024);

    while (running) {
            
        handleQuit();
        if (menuRunning){
            if (menu.isShown()) { 
                menu.draw(renderer);
            }
        }
    }
    
    try {
        if (menu_thread.joinable())
            menu_thread.join();
    } catch (const std::system_error& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    try{
        if(quit_thread.joinable())
            quit_thread.join();
    }catch(const std::system_error& e){
        std::cerr << "Exception caught 2: " << e.what() << std::endl;
    }
    if (!isPlayingOnline) {
        cleanup();
    }
    //pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(argc, args);
    return 0;
}


int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(int argc, char* args[]) {
    // if (!initSDL()) {
    //     std::cerr << "Failed to initialize!" << std::endl;
    //     return -1;
    // }

    game_running = true;

    std::vector<Kelp> kelps;
    std::vector<Rock> rocks;
    std::vector<Coral> corals;
    generateProceduralDecorations(kelps, rocks, corals, ENV_HEIGHT, ENV_WIDTH, renderer);

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    for (int i = 0; i < FISH_NUMBER ; ++i) {
        school.emplace_back(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 75, 75, renderer, rand() % 2 == 0 ? 1 : 0, fishTextures[rand() % fishCount]);
    }
    std::ranges::sort(school, Fish::SortByX);
    std::vector<std::thread> fish_threads;
    int fishPerThread = school.size() / std::thread::hardware_concurrency();
    for (int i = 0; i < school.size(); i += fishPerThread) {
        fish_threads.emplace_back(createThread("Fish thread", updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size()))));
    }
    //std::thread quit_thread = createThread("Quit thread", handleQuitThread);
  
      // Offline
    players.emplace_back(Player(windowWidth / 2, windowHeight / 2, 5, renderer, 0));
    std::thread player_thread = createThread("Player thread", playerMovementThread, std::ref(players[0]));


    Shark shark(0, 0, 0.1, 0.1,0, 150, 150, renderer,players);
    std::thread shark_thread = createThread("Shark thread",updateShark, std::ref(shark));

    while (game_running) {
        renderScene(players, kelps, rocks, corals, shark);
        //handleQuit();

    }
    try{
        if(player_thread.joinable()){
            std::cout << "Killing playerThread.." << std::endl;
            player_thread.join();
        }
    }catch(const std::system_error& e){
        std::cerr << "Exception caught 1: " << e.what() << std::endl;
    }
    try {
      std::cout << "Killing fish_threads..." << std::endl;
      for (auto& fish_thread : fish_threads) {
          if (fish_thread.joinable())
              fish_thread.join();
      }
    } catch (const std::system_error& e) {
        std::cerr << "Exception caught 4: " << e.what() << std::endl;
    }

    try {
        if (shark_thread.joinable()){
            std::cout << "Killing shark_thread..." << std::endl;
            shark_thread.join();
        }
    } catch (const std::system_error& e) {
        std::cerr << "Exception caught 5: " << e.what() << std::endl;
    }
    std::cout << "All threads killed" << std::endl;
    running = false;
    return 0;
}

int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(int argc, std::string args) {
    // if (!initSDL()) {
    //     std::cerr << "Failed to initialize!" << std::endl;
    //     return -1;
    // }
    game_running = true;
    std::vector<Kelp> kelps;
    std::vector<Rock> rocks;
    std::vector<Coral> corals;
    generateProceduralDecorations(kelps, rocks, corals, ENV_HEIGHT, ENV_WIDTH, renderer);

    for (int i = 0; i < FISH_NUMBER; ++i) {
        school.emplace_back(Fish(rand() % ENV_WIDTH, rand() % ENV_HEIGHT, 0.1, 0.1, school, i, 75, 75, renderer, rand() % 2 == 0 ? 1 : 0, fishTextures[rand() % fishCount]));
    }
    std::cout << "Thread: " << std::thread::hardware_concurrency() << std::endl;
    std::vector<std::thread> threads;
    int fishPerThread = school.size() / std::thread::hardware_concurrency();
    int thread_id = 0;
    for (int i = 0; i < school.size(); i += fishPerThread) {
        threads.emplace_back(createThread("Fish thread", updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size()))));
    }

    Shark shark(0, 0, 0.1, 0.1,0, 150, 150, renderer,players);
    std::thread shark_thread = createThread("Shark", updateShark, std::ref(shark));

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    if (isPlayingOnline) {
        if (argc == 0 && args == "") {
            if (!initServer()) {
                std::cerr << "Failed to initialize server!" << std::endl;
                return -1;
            }
            isHost = true;
            std::cout << "isHost: " << isHost << std::endl;
            std::thread acceptThread = createThread("Accept thread", acceptClients);
            IPaddress ip;
            if (!initClient(ip, "localhost", 1234)) {
                std::cerr << "Failed to initialize client!" << std::endl;
                return -1;
            }
            players.emplace_back(Player(windowWidth / 2, windowHeight / 2, 5, renderer, 0));
            std::ranges::sort(school, Fish::SortByX);
            std::vector<std::thread> fish_threads;
            int fishPerThread = school.size() / std::thread::hardware_concurrency();
            for (int i = 0; i < school.size(); i += fishPerThread) {
                fish_threads.emplace_back(createThread("Fish thread", updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size()))));
            }
            messageThreadRunning = true;
            std::thread messageThread = createThread("Message thread", handleClientMessages, std::ref(players[0]));
            std::thread playerThread = createThread("Player thread", playerMovementThread, std::ref(players[0]));

            while (game_running) {
                renderScene(players, kelps, rocks, corals,shark);
                SDL_Delay(10);
            }
            messageThreadRunning = false;
            try{
                //if(playerThread.joinable())
                std::cout << "Killing playerThread.." << std::endl;
                playerThread.join();
                std::cout << "playerThread killed" << std::endl;

            }catch(const std::system_error& e){
                std::cerr << "Exception caught 1: " << e.what() << std::endl;
            }
            try {
                for (auto& fish_thread : fish_threads) {
                    if (fish_thread.joinable())
                        fish_thread.join();
                }
            } catch (const std::system_error& e) {
                std::cerr << "Exception caught 2: " << e.what() << std::endl;
            }
            for (auto& thread : threads) {
                try {
                    std::cout << "Killing thread..." << std::endl;
                    thread.join();
                    std::cout << "Thread killed" << std::endl;
                } catch (const std::system_error& e) {
                    std::cerr << "Exception caught 3: " << e.what() << std::endl;
                }
            }
            try {
                //if (messageThread.joinable())
                closeClient();
                std::cout << "Killing messageThread" << std::endl;
                messageThread.join();
                std::cout << "messageThread killed" << std::endl;
            } catch (const std::system_error& e) {
                std::cerr << "Exception caught 4: " << e.what() << std::endl;
            }
            try {
                if (acceptThread.joinable()){
                    std::cout << "Killing acceptThread" << std::endl;
                    acceptThread.join();
                    std::cout << "acceptThread killed" << std::endl;
                }
            } catch (const std::system_error& e) {
                std::cerr << "Exception caught 5: " << e.what() << std::endl;
            }

            running = false;
        }
        else if (argc > 0 && argc < 65535 && args != "") {
            int port = 1234;
            std::string host = args;
            if (!initClient(ip, host.c_str(), 1234)) {
                std::cerr << "Failed to initialize client!" << std::endl;
                return -1;
            }
            /*
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> disX(0, ENV_WIDTH);
            std::uniform_int_distribution<> disY(0, ENV_HEIGHT);
            players.emplace_back(Player(disX(gen), disY(gen), 5, renderer, 1));
            */
            players.emplace_back(Player(windowWidth / 2, windowHeight / 2, 5, renderer, 1));
            std::ranges::sort(school, Fish::SortByX);
            std::vector<std::thread> fish_threads;
            int fishPerThread = school.size() / std::thread::hardware_concurrency();
            for (int i = 0; i < school.size(); i += fishPerThread) {
                fish_threads.emplace_back(createThread("Fish thread", updateFishRange, std::ref(school), i, std::min(i + fishPerThread, static_cast<int>(school.size()))));
            }
            messageThreadRunning = true;
            std::thread messageThread = createThread("Message thread", handleClientMessages, std::ref(players[0]));
            std::thread playerThread = createThread("Player thread", playerMovementThread, std::ref(players[0]));

            while (running) {
                renderScene(players, kelps, rocks, corals,shark);
                SDL_Delay(10);
            }
            messageThreadRunning = false;
            try{
                //if(playerThread.joinable())
                std::cout << "Killing playerThread.." << std::endl;
                playerThread.join();
                std::cout << "playerThread killed" << std::endl;

            }catch(const std::system_error& e){
                std::cerr << "Exception caught 1: " << e.what() << std::endl;
            }
            try {
                for (auto& fish_thread : fish_threads) {
                    if (fish_thread.joinable())
                        fish_thread.join();
                }
            } catch (const std::system_error& e) {
                std::cerr << "Exception caught 2: " << e.what() << std::endl;
            }
            for (auto& thread : threads) {
                try {
                    std::cout << "Killing thread..." << std::endl;
                    thread.join();
                    std::cout << "Thread killed" << std::endl;
                } catch (const std::system_error& e) {
                    std::cerr << "Exception caught 3: " << e.what() << std::endl;
                }
            }
            try {
                //if (messageThread.joinable())
                closeClient();
                std::cout << "Killing messageThread" << std::endl;
                messageThread.join();
                std::cout << "messageThread killed" << std::endl;
            } catch (const std::system_error& e) {
                std::cerr << "Exception caught 4: " << e.what() << std::endl;
            }
            running = false;
        }
    }

    return 0;
}


void handleQuit() {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            if (isPlayingOnline && isHost) {
                closeServer();
            }
            game_running = false;
        }
    }

    if (keystate[SDL_SCANCODE_ESCAPE]) {
        if (isPlayingOnline && isHost) {
            closeServer();
        }
        game_running = false;
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

    shark.draw(renderer);

    displayFPS(renderer, font, fps);
    for (auto& player : players) {
        auto [playerX, playerY] = player.getPlayerPos();
        displayPlayerCoord(renderer, font, playerX, playerY);
    }

    for (auto& player_server : players_server) {
        int unifiedX = player_server.getUnifiedX();
        int unifiedY = player_server.getUnifiedY();
        displayUnifiedPlayerCoord(renderer, font, unifiedX, unifiedY);
        displayNearbyPlayers(renderer, font, player_server, players_server, 500.0);
    }

    SDL_RenderPresent(renderer);
}

void cleanup() {
    try {
        if (font != nullptr) {
            TTF_CloseFont(font);
            font = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for CloseFont: " << e.what() << std::endl;
    }

    try {
        TTF_Quit();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for TTF_Quit: " << e.what() << std::endl;
    }

    try {
        if (playerTexture != nullptr) {
            SDL_DestroyTexture(playerTexture);
            playerTexture = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for DestroyTexture (playerTexture): " << e.what() << std::endl;
    }

    for (int i = 0; i < fishCount; ++i) {
        try {
            if (fishTextures[i] != nullptr) {
                SDL_DestroyTexture(fishTextures[i]);
                fishTextures[i] = nullptr;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught for DestroyTexture (fishTextures[" << i << "]): " << e.what() << std::endl;
        }
    }

    try {
        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for DestroyRenderer: " << e.what() << std::endl;
    }

    try {
        if (window != nullptr) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for DestroyWindow: " << e.what() << std::endl;
    }

    try {
        IMG_Quit();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for IMG_Quit: " << e.what() << std::endl;
    }

    try {
        SDLNet_Quit();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for SDLNet_Quit: " << e.what() << std::endl;
    }

    try {
        Mix_CloseAudio();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for Mix_CloseAudio: " << e.what() << std::endl;
    }

    try {
        SDL_Quit();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for SDL_Quit: " << e.what() << std::endl;
    }
}

