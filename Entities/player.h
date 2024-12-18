#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <tuple>
#include <vector>
#include <syncstream>
#include <tuple>
#include "Network/networking_client.h"
#include "camera.h"
#include "fish.h"
#include <syncstream>
#include <tuple>
#include "../network/networking_client.h"
#include "../camera.h"


enum playerData {
    PLAYER_SPRITE_1 = 0,
    PLAYER_SPRITE_2 = 1,
    PLAYER_SPRITE_3 = 2,
    PLAYER_SIZE_X = 124,
    PLAYER_SIZE_Y = 124
};


#define playerIMG "../img/player/playerV2-full.png"

class Player {
    private:
        int x, y;
        int playerBaseX, playerBaseY;
        int playerSpeed;
        int currentSprite = PLAYER_SPRITE_1;
        int ticks = 0;
        int playerId;
        float energy = 100.0f;
        SDL_RendererFlip currentFlip = SDL_FLIP_NONE;
        SDL_Texture* playerTexture = nullptr;
        SDL_Rect playerPosForRender = {0, 0, PLAYER_SIZE_X, PLAYER_SIZE_Y};
        SDL_Rect playerRect[3] = {
            {69, 73, 112, 117},
            {326, 73, 112, 117},
            {528, 73, 112, 117}
        };
        Uint32 lastMoveTime;
        bool onlineMovement();
        int unifiedX = 0;
        int unifiedY = 0;
        int HITBOX = 50;
    public:
        Player(int x, int y, int playerSpeed, SDL_Renderer* renderer, int playerId) : x(x), y(y), playerBaseX(x), playerBaseY(y), playerSpeed(playerSpeed), playerId(playerId) {
            playerPosForRender.x = x;
            playerPosForRender.y = y;
            unifiedX = x + windowWidth / 2;
            unifiedY = y + windowHeight / 2;

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
        std::tuple<int, int> getPlayerPos() const;
        int getPlayerSpeed();
        void draw(SDL_Renderer* renderer);
        void handlePlayerMovement(int ENV_WIDTH, int ENV_HEIGHT, int windowWidth, int windowHeight);
        void drawEnergyBar(SDL_Renderer* renderer);
        bool checkCollision(SDL_Rect fishRect);
        int getPlayerId();
        void setPlayerPos(int x, int y);
        void handleClientMessages();
        void updatePosition(int x, int y);
        int getUnifiedX();
        int getUnifiedY();

        int getSpeed();
        SDL_Texture* getTexture() {
            return playerTexture;
        }

        int getHITBOX() const {return HITBOX;}

};

#endif