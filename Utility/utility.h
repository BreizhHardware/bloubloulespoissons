//
// Created by BreizhHardware on 18/12/2024.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <string>
#include "env.h"
#include "../Game/camera.h"
#include "../Entities/fish.h"
#include "../Entities/player.h"
#include "../Entities/shark.h"
#include "../Game/decors.h"
#include "../Utility/display.h"

extern std::mutex mtx;

void displayFPS(SDL_Renderer* renderer, TTF_Font* font, int fps);
void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int playerX, int playerY);
void displayUnifiedPlayerCoord(SDL_Renderer* renderer, TTF_Font* font, int unifiedX, int unifiedY);
void displayPlayerCount(SDL_Renderer* renderer, TTF_Font* font, int playerCount);
void checkThreads();
bool initSDL();
struct ThreadInfo {
    std::thread::id id;
    std::string functionName;
    bool isRunning;
};
extern std::vector<ThreadInfo> threadInfos;
template <typename Function, typename... Args>
std::thread createThread(std::string key, Function&& func, Args&&... args) {
    try {
        std::cout << "Creating thread: " << key << std::endl;
        std::thread thread([key, func = std::forward<Function>(func), ...args = std::forward<Args>(args)]() mutable {
            ThreadInfo info;
            info.id = std::this_thread::get_id();
            info.functionName = key;
            info.isRunning = true;
            {
                std::lock_guard<std::mutex> lock(mtx);
                threadInfos.push_back(info);
            }
            try {
                func(std::forward<Args>(args)...);
            } catch (const std::exception& e) {
                std::cerr << "Exception in thread " << key << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in thread " << key << std::endl;
            }
            {
                std::lock_guard<std::mutex> lock(mtx);
                for (auto& threadInfo : threadInfos) {
                    if (threadInfo.id == std::this_thread::get_id()) {
                        threadInfo.isRunning = false;
                        break;
                    }
                }
            }
            std::cout << "ThreadID = " << info.id << " ThreadFunction = " << info.functionName << " finished" << std::endl;
        });
        return thread;
    } catch (const std::system_error& e) {
        std::cerr << "Failed to create thread: " << e.what() << std::endl;
        throw;
    }
}
void updateFishRange(std::vector<Fish>& school, int start, int end);
void playerMovementThread(Player& player);
void updateShark(Shark &shark);
void renderScene(std::vector<Player>& players, const std::vector<Kelp>& kelps, const std::vector<Rock>& rocks, const std::vector<Coral>& corals,Shark& shark );
void handleClientMessages(Player& player);
#endif //UTILITY_H
