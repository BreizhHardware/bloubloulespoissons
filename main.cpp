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

#include "Entities/fish.h"
#include "Game/decors.h"
#include "Game/camera.h"
#include "Utility/display.h"
#include "Utility/env.h"
#include "Utility/utility.h"
#include "Entities/player.h"
#include "Game/menu.h"
#include "Network/networking.h"
#include "Network/networking_client.h"
#include "Entities/shark.h"
#include "Utility/event.h"
#include "Utility/close.h"
#include "Game/launchGameSolo.h"
#include "Game/launchGameMulti.h"


#include <system_error>

std::atomic<bool> menuRunning(true);

std::mutex coutMutex;
std::vector<Player> players;
std::vector<Player> players_server;

void HandleMenuClick(Menu& menu);

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

