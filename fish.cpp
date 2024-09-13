//
// Created by BreizhHardware on 13/09/2024.
//

#include "fish.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int ENV_WIDTH = 1600;
constexpr int ENV_HEIGHT = 1200;

void Fish::move() {
    x += vx;
    y += vy;
    if(x < 0 || x >= ENV_WIDTH) {
        vx = -vx;
    }
    if(y < 0 || y >= ENV_HEIGHT) {
        vy = -vy;
    }
}

void Fish::draw(SDL_Renderer* renderer) {
    SDL_Rect rect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
    SDL_RenderFillRect(renderer, &rect);
}