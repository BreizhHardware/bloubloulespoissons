//
// Created by BreizhHardware on 18/12/2024.
//

#include "close.h"

void handleQuitThread() {
    std::cout << "handleQuitThread..." << std::endl;
    while (game_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        handleQuit();
    }
    std::cout << "handleQuitThread" << std::endl;
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

    if (keystate[SDL_SCANCODE_M]) {
        soundMuted = !soundMuted;
        if (soundMuted) {
            Mix_Volume(-1, 0);
            Mix_VolumeMusic(0);
        } else {
            Mix_Volume(-1, MIX_MAX_VOLUME);
            Mix_VolumeMusic(MIX_MAX_VOLUME);
        }
    }

    if (keystate[SDL_SCANCODE_F2]) {
        displayFPSFlag = !displayFPSFlag;
    }
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
        if (backgroundMusic != nullptr) {
            Mix_FreeMusic(backgroundMusic);
            backgroundMusic = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for FreeMusic: " << e.what() << std::endl;
    }

    try {
        if (menuMusic != nullptr) {
            Mix_FreeMusic(menuMusic);
            menuMusic = nullptr;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for FreeMusic (menuMusic): " << e.what() << std::endl;
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