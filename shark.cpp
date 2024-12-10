//
// Created by cfouche on 09/12/24.
//

#define sharkIMG "../img/mory.png"
#include "shark.h"

Shark::Shark(const int x, const int y, const float vx, const float vy, const int id, const int width, const int height, SDL_Renderer* renderer, const std::vector<Player> &players_list)
    : x(x), y(y), vx(vx), vy(vy), id(id), width(width), height(height), players_list(players_list) {
    SDL_Surface* sharkSurface = IMG_Load(sharkIMG);
    if (sharkSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image du joueur: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, sharkSurface);
        SDL_FreeSurface(sharkSurface);
    }

}

void Shark::draw(SDL_Renderer *renderer) {
    Camera &camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    SDL_Rect rect = {static_cast<int>(x) - cameraX, static_cast<int>(y) - cameraY, width, height};
    float angle = atan2(vy, vx) * 180 / M_PI; // Convert angle to degrees

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool Shark::isInView(Player& player) {
    std::cout << "Player x: " << player.getUnifiedX() << " y: " << player.getUnifiedY() << std::endl;
    std::cout.flush();
    return player.getUnifiedX() >= x - VISUAL_RANGE && player.getUnifiedX() <= x + VISUAL_RANGE && player.getUnifiedY() >= y - VISUAL_RANGE && player.getUnifiedY() <= y + VISUAL_RANGE;
}

void Shark::checkNeighborhood(Player& player, float &xpos_avg, float &ypos_avg, float &xvel_avg, float &yvel_avg, int &neighboring_player) {
    if (isInView(player)) {
        xpos_avg += player.getUnifiedX();
        ypos_avg += player.getUnifiedY();
        xvel_avg += player.getSpeed();
        yvel_avg += player.getSpeed();
        neighboring_player++;
    }
}


void Shark::cycle() {
    int neighboring_player = 0;
    float xvel_avg = 0, yvel_avg = 0, xpos_avg = 0, ypos_avg = 0;
    std::cout << "Shark x: " << x << " y: " << y << std::endl;
    for (auto& player : players_list) {
        if (isInView(player)) {
            std::cout << "Player x: " << player.getUnifiedX() << " y: " << player.getUnifiedY() << std::endl;
            checkNeighborhood(player, xpos_avg, ypos_avg, xvel_avg, yvel_avg, neighboring_player);
        }
    }
    if (neighboring_player > 0) {
        xpos_avg /= neighboring_player;
        ypos_avg /= neighboring_player;
        xvel_avg /= neighboring_player;
        yvel_avg /= neighboring_player;
        vx += (xpos_avg - x) * CENTERING_FACTOR + (xvel_avg - vx) * MATCHING_FACTOR;
        vy += (ypos_avg - y) * CENTERING_FACTOR + (yvel_avg - vy) * MATCHING_FACTOR;
    }

    float speed = sqrt(vx * vx + vy * vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    } else if (speed < MIN_SPEED) {
        vx = vx / speed * MIN_SPEED;
        vy = vy / speed * MIN_SPEED;
    }
    x += vx;
    y += vy;

}