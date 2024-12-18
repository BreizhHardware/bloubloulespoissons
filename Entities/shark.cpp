//
// Created by cfouche on 09/12/24.
//

#define sharkIMG "../img/shark.png"
#include "shark.h"

Shark::Shark(const int x, const int y, const float vx, const float vy, const int id, const int width, const int height, SDL_Renderer* renderer, std::vector<Player> &players_list)
    : x(x), y(y), vx(vx), vy(vy), id(id), width(width), height(height), players_list(players_list) {
    SDL_Surface* sharkSurface = IMG_Load(sharkIMG);
    if (sharkSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image du joueur: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, sharkSurface);
        SDL_FreeSurface(sharkSurface);
    }

    // sharkSound = Mix_LoadWAV("../sounds/shark.wav");
    // if (sharkSound == nullptr) {
    //     std::cerr << "Erreur de chargement du son du requin: " << Mix_GetError() << std::endl;
    // }

    // approachingSound = Mix_LoadWAV("../sounds/Shark-approching.wav");
    // if (approachingSound == nullptr) {
    //     std::cerr << "Erreur de chargement du son d'approche du requin: " << Mix_GetError() << std::endl;
    // }

    lastSoundTime = std::chrono::steady_clock::now();
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

void Shark::checkCollision(Player& player) {
    if (player.getUnifiedX() + player.getHITBOX() >= x - HITBOX && player.getUnifiedX() - player.getHITBOX()  <= x + HITBOX && player.getUnifiedY() + player.getHITBOX() >= y - HITBOX && player.getUnifiedY() - player.getHITBOX() <= y + HITBOX) {
       game_running = false;
    }
}


void Shark::cycle() {
    int neighboring_player = 0;
    float xvel_avg = 0, yvel_avg = 0, xpos_avg = 0, ypos_avg = 0;
    for (auto& player : players_list) {
        if (isInView(player)) {
            checkNeighborhood(player, xpos_avg, ypos_avg, xvel_avg, yvel_avg, neighboring_player);
            checkCollision(player);
            //Mix_PlayChannel(SOUND_CHANNEL, approachingSound, 0);
            musicManager.playMusic("Shark-approaching");
            musicManager.pauseMusic("Playing");
        }
        if (!isInView(player)) {
            // Cut the approach sound
            musicManager.stopMusic("Shark-approaching");
            musicManager.resumeMusic("Playing");
            // Mix_HaltChannel(SOUND_CHANNEL);
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

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSoundTime).count() > 15) {
        // if (sharkSound != nullptr) {
        //     Mix_PlayChannel(SOUND_CHANNEL, sharkSound, 0);
        // }
        musicManager.playMusic("Shark");
        lastSoundTime = now;
    }

    if (isPlayingOnline && isHost) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime).count() >= 100) {
            sendSharkPosition(client, id, x, y);
            lastSendTime = now;
        }
    }
}

Shark::~Shark() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    // if (sharkSound) {
    //     Mix_FreeChunk(sharkSound);
    //     sharkSound = nullptr;
    // }

    // if (approachingSound) {
    //     Mix_FreeChunk(approachingSound);
    //     approachingSound = nullptr;
    // }
}

void Shark::updatePosition(int newX, int newY) {
    x = newX;
    y = newY;
}
