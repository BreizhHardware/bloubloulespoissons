//
// Created by BreizhHardware on 13/09/2024.
//

#include "decors.h"
#include <iostream>

#include "env.h"

void Rock::draw(SDL_Renderer* renderer) const{
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    for (int w = 0; w < size * 2; w++) {
        for (int h = 0; h < size * 2; h++) {
            int dx = size - w;
            int dy = size - h;
            if ((dx * dx + dy * dy) <= (size * size)) {
                SDL_Rect rect = { x + dx - cameraX, y + dy - cameraY, 1, 1 };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    //std::cout << "Rock drawn at (" << x << ", " << y << ")" << std::endl;
}


void Kelp::draw(SDL_Renderer* renderer) const {
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();
    SDL_Rect kelpRect = { x - cameraX, y - cameraY, size / 3, size };
    if (texturesVector[0] != nullptr) {
        SDL_RenderCopy(renderer, texturesVector[0], nullptr, &kelpRect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &kelpRect);
    }
}

void generateProceduralDecorations(std::vector<Kelp>&kelps, std::vector<Rock>&rocks, int envHeight, int envWidth , SDL_Renderer* renderer) {
    std::srand(std::time(0));

    //Generate Kelp
    int numKelps = 10 + std::rand() % 20;
    for (int i = 0; i < numKelps; i++) {
        int x = std::rand() % envWidth;
        int y = envHeight - (std::rand() % (envHeight / 10));
        int size = 50 + std::rand() % 100;
        Uint8 r = 4;
        Uint8 g = 87;
        Uint8 b = 0;
        kelps.emplace_back(Kelp(x, y, size, r, g, b, renderer));
    }

    //Generate Rocks
    int numRocks = 10 + std::rand() % 20;
    for (int i = 0; i < numRocks; i++) {
        int x = std::rand() % envWidth;
        int y = envHeight - (std::rand() % (envHeight / 10));
        int size = 10 + std::rand() % 20;
        int r = 100 + std::rand() % 156;
        int g = 100 + std::rand() % 156;
        int b = 100 + std::rand() % 156;
        rocks.emplace_back(Rock(x, y, size, r, g, b));
    }
}
