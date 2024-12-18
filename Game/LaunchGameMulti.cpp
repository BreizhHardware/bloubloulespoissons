//
// Created by BreizhHardware on 18/12/2024.
//

#include "LaunchGameMulti.h"

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
            school.clear();
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
            school.clear();
            players.clear();
            eventHandler.triggerEvent("playerLost");
        }
    }

    return 0;
}