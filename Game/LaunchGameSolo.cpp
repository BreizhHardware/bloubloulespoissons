//
// Created by BreizhHardware on 18/12/2024.
//

#include "LaunchGameSolo.h"

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
        int textureIndex = rand() % fishCount;
        int gender = rand() % 2 == 0 ? 1 : 0;

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
    school.clear();
    players.clear();
    eventHandler.triggerEvent("playerLost");
    return 0;
}