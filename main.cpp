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
#include "Utility/display.h"
#include "Utility/env.h"
#include "Utility/utility.h"
#include "player.h"
#include "menu.h"
#include "Network/networking.h"
#include "Network/networking_client.h"
#include "shark.h"
#include "Utility/event.h"
#include "Utility/close.h"



#include <system_error>

std::atomic<bool> menuRunning(true);

std::mutex coutMutex;
std::vector<Fish> school;
std::vector<Player> players;
std::vector<Player> players_server;

void renderScene(std::vector<Player>& players, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals,Shark& shark );

void playerMovementThread(Player& player);
void handleClientMessages(Player& player);
void HandleMenuClick(Menu& menu);
void updateFishRange(std::vector<Fish>& school, int start, int end);
int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(int argc, char* args[]);
int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(int argc, std::string args);

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

void HandleMenuClick(Menu& menu){
    try {
        while (menuRunning) {
            menu.handleClickedButton();
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in HandleMenuClick: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in HandleMenuClick" << std::endl;
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

void onPlayerLost(Menu &menu){
    //Affiche drawLost par dessus le jeu
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    menu.drawLost(renderer);
    SDL_RenderPresent(renderer);
    Mix_Chunk* deathSound = Mix_LoadWAV("../sounds/death.wav");
    Mix_PlayChannel(-1, deathSound, 0);
    std::this_thread::sleep_for(std::chrono::seconds(8));
    Mix_FreeChunk(deathSound);
    menuRunning = true;
    menu.changePage("Main");
    menu.show();
    resetAll();
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

    
    eventHandler.registerEvent("playerLost", onPlayerLost, std::ref(menu));

    std::thread menu_thread = createThread("Menu thread", HandleMenuClick, std::ref(menu));
    std::thread quit_thread = createThread("Quit thread", handleQuitThread);

    menu.addText("Main", (windowWidth/2) - 300, 50, 600, 100, "BloubBloub les poissons", 1024);

    menu.addImage("Main", (windowWidth/2) - 100, 150, 200, 200, "../img/logo.png");

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

    menuMusic = Mix_LoadMUS("../sounds/Menu.wav");
    if (menuMusic != nullptr) {
        Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
        if (Mix_PlayMusic(menuMusic, -1) == -1) {
            std::cerr << "Erreur de lecture de la musique du menu: " << Mix_GetError() << std::endl;
        }
    }

    while (running) {
            
        handleQuit();
        if (menuRunning){
            if (menu.isShown()) { 
                menu.draw(renderer);
            }
        }
    }

    std::cout << "Check Threads 1" << std::endl;
    checkThreads();
    try {
        menuRunning = false;
        if (menu_thread.joinable()) {
            menuRunning = false;
            menu_thread.join();
        }
    } catch (const std::system_error& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    checkThreads();
    try{
        if(quit_thread.joinable())
            quit_thread.join();
    }catch(const std::system_error& e){
        std::cerr << "Exception caught 2: " << e.what() << std::endl;
    }

    if (!isPlayingOnline) {
        cleanup();
    }

    std::cout << "Check Threads 2" << std::endl;
    checkThreads();

    //pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(argc, args);
    return 0;
}


int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons(int argc, char* args[]) {
    // if (!initSDL()) {
    //     std::cerr << "Failed to initialize!" << std::endl;
    //     return -1;
    // }

    game_running = true;

    Mix_HaltMusic();

    backgroundMusic = Mix_LoadMUS("../sounds/Playing.wav");
    if (backgroundMusic == nullptr) {
        std::cerr << "Erreur de chargement de la musique: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        std::cerr << "Erreur de lecture de la musique: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(backgroundMusic, -1);

    std::vector<Kelp> kelps;
    std::vector<Rock> rocks;
    std::vector<Coral> corals;
    generateProceduralDecorations(kelps, rocks, corals, ENV_HEIGHT, ENV_WIDTH, renderer);

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    for (int i = 0; i < FISH_NUMBER; ++i) {
        int x = rand() % ENV_WIDTH;
        int y = rand() % ENV_HEIGHT;
        double speedX = 0.1;
        double speedY = 0.1;
        std::cout << fishCount << std::endl;
        int textureIndex = rand() % fishCount;
        int gender = rand() % 2 == 0 ? 1 : 0;

        std::cout << "Creating fish " << i << " at (" << x << ", " << y << ") with texture " << textureIndex << " and gender " << gender << std::endl;

        school.emplace_back(x, y, speedX, speedY, school, i, 75, 75, renderer, gender, fishTextures[textureIndex]);
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
    // running = false;
    school.empty();
    players.clear();
    eventHandler.triggerEvent("playerLost");
    return 0;
}

int pas_la_fontion_main_enfin_ce_nest_pas_la_fontion_principale_du_programme_mais_une_des_fonctions_principale_meme_primordiale_du_projet_denomme_bloubloulespoissons_mais_celle_ci_elle_lance_en_multijoueur(int argc, std::string args) {
    // if (!initSDL()) {
    //     std::cerr << "Failed to initialize!" << std::endl;
    //     return -1;
    // }
    game_running = true;

    Mix_HaltMusic();

    backgroundMusic = Mix_LoadMUS("../sounds/Playing.wav");
    if (backgroundMusic == nullptr) {
        std::cerr << "Erreur de chargement de la musique: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        std::cerr << "Erreur de lecture de la musique: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(backgroundMusic, -1);

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
            school.empty();
            players.clear();
            eventHandler.triggerEvent("playerLost");
        }
        else if (argc > 0 && argc < 65535 && args != "") {
            int port = 1234;
            std::string host = args;
            if (!initClient(ip, host.c_str(), 1234)) {
                std::cerr << "Failed to initialize client!" << std::endl;
                return -1;
            }
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
            school.empty();
            players.clear();
            eventHandler.triggerEvent("playerLost");
        }
    }

    return 0;
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

    SDL_RenderPresent(renderer);
}



