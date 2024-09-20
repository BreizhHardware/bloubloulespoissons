//
// Created by BreizhHardware on 13/09/2024.
//

#include "decors.h"

void Rock::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    for (int w = 0; w < size * 2; w++) {
        for (int h = 0; h < size * 2; h++) {
            int dx = size - w;
            int dy = size - h;
            if ((dx * dx + dy * dy) <= (size * size)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

Reef::Reef(int x, int y) : x(x), y(y) {
    std::srand(std::time(0));
    int numRocks = 2 + std::rand() % 7;
    for (int i = 0; i < numRocks; i++) {
        int size = 50 + std::rand() % 51;
        int r = 46 + std::rand() % 47;
        int g = 45 + std::rand() % 40;
        int b = 45 + std::rand() % 26;
        rocks.emplace_back(x + std::rand() % 100, y + std::rand() % 100, size, r, g, b);
    }
}

void Reef::draw(SDL_Renderer* renderer) {
    for (auto& rock : rocks) {
        rock.draw(renderer);
    }
}

void Kelp::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect kelpRect = { x, y, size / 3, size };
    SDL_RenderFillRect(renderer, &kelpRect);
}