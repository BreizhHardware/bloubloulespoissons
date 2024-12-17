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
    SDL_Rect rockRect = { x - cameraX, y - cameraY, size, size };
    if (texturesVector[1] != nullptr) {
        SDL_RenderCopy(renderer, texturesVector[1], nullptr, &rockRect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rockRect);
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

void Coral::draw(SDL_Renderer* renderer) const {
    Camera& camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();
    SDL_Rect coralRect = { x - cameraX, y - cameraY, size, size };
    if (texturesVector[2] != nullptr) {
        SDL_RenderCopy(renderer, texturesVector[2], nullptr, &coralRect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &coralRect);
    }
}

void generateProceduralDecorations(std::vector<Kelp>& kelps, std::vector<Rock>& rocks, std::vector<Coral>& corals,int envHeight, int envWidth, SDL_Renderer* renderer) {
    std::srand(std::time(0));

    //Generate Kelp
    int numKelps = 10 + std::rand() % 20;
    for (int i = 0; i < numKelps; i++) {
        int x = std::rand() % envWidth;
        int y = envHeight - (std::rand() % static_cast<int>(envHeight * 0.2));
        int size = 250 + std::rand() % 100;
        Uint8 r = 4;
        Uint8 g = 87;
        Uint8 b = 0;
        kelps.emplace_back(Kelp(x, y, size, r, g, b, renderer));
    }

    //Generate Rocks
    int numRocks = 10 + std::rand() % 10;
    for (int i = 0; i < numRocks; i++) {
        int x = std::rand() % envWidth;
        int y = envHeight - (std::rand() % static_cast<int>(envHeight * 0.2));
        int size = 250 + std::rand() % 20;
        int r = 100 + std::rand() % 156;
        int g = 100 + std::rand() % 156;
        int b = 100 + std::rand() % 156;
        rocks.emplace_back(Rock(x, y, size, r, g, b, renderer));
    }

    int numCoral = 10 + std::rand() % 10;
    for (int i = 0; i < numCoral; i++) {
        int x = std::rand() % envWidth;
        int y = envHeight - (std::rand() % static_cast<int>(envHeight * 0.2));
        int size = 250 + std::rand() % 20;
        int r = 100 + std::rand() % 156;
        int g = 100 + std::rand() % 156;
        int b = 100 + std::rand() % 156;
        corals.emplace_back(Coral(x, y, size, r, g, b, renderer));
    }
}
