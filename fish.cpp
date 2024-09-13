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

void Fish::drawArrow(SDL_Renderer* renderer, int x, int y, int vx, int vy) {
    const int arrowLength = 30; // Augmenter la longueur de la flèche
    const int arrowWidth = 10;  // Garder la largeur de la flèche constante
    SDL_Point points[4];

    if (vx == 0 && vy == 0) return;

    float angle = atan2(vy, vx);
    points[0] = { x + static_cast<int>(arrowLength * cos(angle)), y + static_cast<int>(arrowLength * sin(angle)) };
    points[1] = { x + static_cast<int>(arrowWidth * cos(angle + M_PI / 6)), y + static_cast<int>(arrowWidth * sin(angle + M_PI / 6)) };
    points[2] = { x + static_cast<int>(arrowWidth * cos(angle - M_PI / 6)), y + static_cast<int>(arrowWidth * sin(angle - M_PI / 6)) };
    points[3] = points[0];

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
    SDL_RenderDrawLines(renderer, points, 4);
}


void Fish::draw(SDL_Renderer* renderer) {
    SDL_Rect rect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
    SDL_RenderFillRect(renderer, &rect);

    drawArrow(renderer, x + width / 2, y + height / 2, vx, vy);
}