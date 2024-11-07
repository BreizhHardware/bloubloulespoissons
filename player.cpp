#include "player.h"
#include <tuple>

void Player::updatePlayerPos(int x, int y) {
    this->x = x;
    this->y = y;
    this->playerPosForRender.x = x;
    this->playerPosForRender.y = y;
};

void Player::updatePlayerSpeed(int playerSpeed) {
    this->playerSpeed = playerSpeed;
};

auto Player::getPlayerPos() {
    return std::make_tuple(this->x, this->y);
};

int Player::getPlayerSpeed() {
    return playerSpeed;
};


void Player::draw(SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, this->playerTexture, &this->playerRect[this->currentSprite], &this->playerPosForRender, 0, nullptr, SDL_FLIP_NONE);
};