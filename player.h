#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <tuple>


enum playerData {
    PLAYER_SPRITE_1 = 0,
    PLAYER_SPRITE_2 = 5,
    PLAYER_SPRITE_3 = 8,
    PLAYER_SPRITE_4 = 11,
    PLAYER_SPRITE_5 = 14,
    PLAYER_SIZE_X = 75,
    PLAYER_SIZE_Y = 75
};


#define playerIMG "../img/player.png"

class Player {
    private:
        int x, y;
        int playerBaseX, playerBaseY;
        int playerSpeed;
        int currentSprite = 0;
        SDL_Texture* playerTexture = nullptr;
        SDL_Rect playerPosForRender = {0, 0, PLAYER_SIZE_X, PLAYER_SIZE_Y};
        SDL_Rect playerRect[5] = {
            {0, 0, 513, 600},
            {46, 26, 442, 541},
            {560, 23, 426, 536},
            {986, 23, 469, 530},
            {1436, 23, 465, 520}
        };

    public:
        Player(int x, int y, int playerSpeed, SDL_Renderer* renderer) : x(x), y(y), playerBaseX(x), playerBaseY(y), playerSpeed(playerSpeed) {
            playerPosForRender.x = x;
            playerPosForRender.y = y;

            SDL_Surface* playerSurface = IMG_Load(playerIMG);
            if (playerSurface == nullptr) {
                std::cerr << "Erreur de chargement de l'image du joueur: " << IMG_GetError() << std::endl;
            } else {
                playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
                SDL_FreeSurface(playerSurface);
            }

        };
        void updatePlayerPos(int x, int y);
        void updatePlayerSpeed(int playerSpeed);
        std::tuple<int, int> getPlayerPos();
        int getPlayerSpeed();
        void draw(SDL_Renderer* renderer);
        void handlePlayerMovement(int ENV_WIDTH, int ENV_HEIGHT, int windowWidth, int windowHeight);
};

#endif