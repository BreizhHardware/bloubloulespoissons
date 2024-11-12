#include "player.h"
#include <tuple>
#include "camera.h"

void Player::updatePlayerPos(int x, int y) {
    this->x = x;
    this->y = y;
    this->playerPosForRender.x = x;
    this->playerPosForRender.y = y;
};

void Player::updatePlayerSpeed(int playerSpeed) {
    this->playerSpeed = playerSpeed;
};

std::tuple<int, int> Player::getPlayerPos() {
    return std::make_tuple(this->x, this->y);
};

int Player::getPlayerSpeed() {
    return playerSpeed;
};


void Player::draw(SDL_Renderer* renderer) {
    switch (this->ticks) {
        case 0:
            this->currentSprite = PLAYER_SPRITE_1;
            break;
        case 5:
            this->currentSprite = PLAYER_SPRITE_2;
            break;
        case 8:
            this->currentSprite = PLAYER_SPRITE_3;
            break;
        default:
            break;
    }

    if (this->ticks >= 8) {
        this->ticks = 0;
    } else {
        this->ticks++;
    }

    SDL_RenderCopyEx(renderer, this->playerTexture, &this->playerRect[this->currentSprite], &this->playerPosForRender, 0, nullptr, this->currentFlip);

    this->drawEnergyBar(renderer);
};

void Player::handlePlayerMovement(int ENV_WIDTH, int ENV_HEIGHT, int windowWidth, int windowHeight) {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    Camera& camera = Camera::getInstance();

    int tempX = this->x;
    int tempY = this->y;
    int speed = this->playerSpeed;
    bool isSprinting = false;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        speed = this->playerSpeed * 2;
        isSprinting = true;
    }

    bool moved = false;
    if (this->energy != 0){
        if (keystate[SDL_SCANCODE_W]) {
            if (camera.getY() > 0 && tempY == this->playerBaseY) {
                camera.move(0, -speed);
            } else if (tempY > 0) {
                tempY -= speed;
            }
            moved = true;
        }
        if (keystate[SDL_SCANCODE_S]) {
            if ((camera.getY() < ENV_HEIGHT - windowHeight) && (tempY == this->playerBaseY)) {
                camera.move(0, speed);
            } else if (tempY < windowHeight - PLAYER_SIZE_Y) {
                tempY += speed;
            }
            moved = true;
        }
        if (keystate[SDL_SCANCODE_A]) {
            if (camera.getX() > 0 && (tempX == this->playerBaseX)) {
                camera.move(-speed, 0);
                this->currentFlip = SDL_FLIP_HORIZONTAL;
            } else if (tempX > 0) {
                tempX -= speed;
                this->currentFlip = SDL_FLIP_HORIZONTAL;
            }
            moved = true;
        }
        if (keystate[SDL_SCANCODE_D]) {
            if (camera.getX() < ENV_WIDTH - windowWidth && (tempX == this->playerBaseX)) {
                camera.move(speed, 0);
                this->currentFlip = SDL_FLIP_NONE;
            } else if (tempX < windowWidth - PLAYER_SIZE_X) {
                tempX += speed;
                this->currentFlip = SDL_FLIP_NONE;
            }
            moved = true;
        }

        if (tempX < 0) {
            tempX = 0;
        } else if (tempX > ENV_WIDTH) {
            tempX = ENV_WIDTH;
        }

        if (tempY < 0) {
            tempY = 0;
        } else if (tempY > ENV_HEIGHT) {
            tempY = ENV_HEIGHT;
        }

        this->updatePlayerPos(tempX, tempY);
    }
    if (moved) {
        lastMoveTime = SDL_GetTicks();
        if (!isSprinting) {
            this->energy -= 0.1f;
        } else {
            this->energy -= 0.2f;
        }
        if (this->energy < 0) {
            this->energy = 0;
        }
    } else {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= 5000) {
            this->energy += 0.2f;
            if (this->energy > 100.0f) {
                this->energy = 100.0f;
            }
        }
    }
}

void Player::drawEnergyBar(SDL_Renderer* renderer) {
    SDL_Rect energyBarBackground = {10, 10, 200, 20};
    SDL_Rect energyBarForeground = {10, 10, static_cast<int>(2 * this->energy), 20};

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &energyBarBackground);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &energyBarForeground);
}