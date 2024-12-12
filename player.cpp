#include "player.h"

#include <syncstream>
#include <tuple>
#include "network/networking_client.h"
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

std::tuple<int, int> Player::getPlayerPos() const {
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
    if (this->energy != 0) {
        if (isPlayingOnline) {
            if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D]) {
                moved = onlineMovement();
            }
        } else {
            if (keystate[SDL_SCANCODE_W]) {
                if (camera.getY() > 0 && tempY == this->playerBaseY) {
                    camera.move(0, -speed);
                    unifiedX = camera.getX() + this->x;
                } else if (tempY > 0) {
                    tempY -= speed;
                    unifiedX = camera.getX() + this->x;
                }
                moved = true;
            }
            if (keystate[SDL_SCANCODE_S]) {
                if ((camera.getY() < ENV_HEIGHT - windowHeight) && (tempY == this->playerBaseY)) {
                    camera.move(0, speed);
                    unifiedX = camera.getX() + this->x;
                } else if (tempY < windowHeight - PLAYER_SIZE_Y) {
                    tempY += speed;
                    unifiedX = camera.getX() + this->x;
                }
                moved = true;
            }
            if (keystate[SDL_SCANCODE_A]) {
                if (camera.getX() > 0 && (tempX == this->playerBaseX)) {
                    camera.move(-speed, 0);
                    this->currentFlip = SDL_FLIP_HORIZONTAL;
                    unifiedY = camera.getY() + this->y;
                } else if (tempX > 0) {
                    tempX -= speed;
                    this->currentFlip = SDL_FLIP_HORIZONTAL;
                    unifiedY = camera.getY() + this->y;
                }
                moved = true;
            }
            if (keystate[SDL_SCANCODE_D]) {
                if (camera.getX() < ENV_WIDTH - windowWidth && (tempX == this->playerBaseX)) {
                    camera.move(speed, 0);
                    this->currentFlip = SDL_FLIP_NONE;
                    unifiedY = camera.getY() + this->y;
                } else if (tempX < windowWidth - PLAYER_SIZE_X) {
                    tempX += speed;
                    this->currentFlip = SDL_FLIP_NONE;
                    unifiedY = camera.getY() + this->y;
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
        unifiedX = camera.getX() + this->x;
        unifiedY = camera.getY() + this->y;
    } else {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= 1000) {
            if (this->energy > 50.0f) {
                this->energy += 0.4f; // Récupération plus rapide si l'énergie est élevée
            } else {
                this->energy += 0.2f; // Récupération plus lente si l'énergie est faible
            }
            if (this->energy > 100.0f) {
                this->energy = 100.0f;
            }
        }
        unifiedX = camera.getX() + this->x;
        unifiedY = camera.getY() + this->y;
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

int Player::getPlayerId() {
    return playerId;
}

void Player::setPlayerPos(int x, int y) {
    this->x = x;
    this->y = y;
    this->playerPosForRender.x = x;
    this->playerPosForRender.y = y;
}

void Player::handleClientMessages() {
    std::string message = receiveMessage(client);
    if (!message.empty()) {
        std::cout << "Client received: " << message << std::endl;
        if (message == "host;quit") {
            std::cout << "Host has quit. Closing client..." << std::endl;
            game_running = false;
            return;
        }
        if (message.find(";moved;") != std::string::npos) {
            int clientId, xCam, yCam;
            sscanf(message.c_str(), "%d;moved;%d,%d", &clientId, &xCam, &yCam);
            if (clientId == playerId) {
                this->setPlayerPos(750, 400);
                Camera& camera = Camera::getInstance();
                if (xCam >= 0 && xCam <= ENV_WIDTH - windowWidth) {
                    if ( yCam >= 0 && yCam <= ENV_HEIGHT - windowHeight) {
                        camera.setPosition(xCam, yCam);
                        updatePosition(xCam, yCam);
                    }
                }
            }
        }
    }
}

bool Player::onlineMovement() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    std::string message = std::to_string(getPlayerId()) + ";move;";
    bool moved = false;
    if (keystate[SDL_SCANCODE_W]) {
        message += "up-";
        moved = true;
    }
    if (keystate[SDL_SCANCODE_S]) {
        message += "down-";
        moved = true;
    }
    if (keystate[SDL_SCANCODE_A]) {
        message += "left-";
        moved = true;
    }
    if (keystate[SDL_SCANCODE_D]) {
        message += "right-";
        moved = true;
    }
    if (moved) { 
        sendMessage(client, message);
        return true;
    }
    return false;
}

void Player::updatePosition(int x, int y) {
    int camX = x + windowWidth / 2;
    int camY = y + windowHeight / 2;
    unifiedX = camX;
    unifiedY = camY;
}

int Player::getUnifiedX() {
    return unifiedX;
}

int Player::getUnifiedY() {
    return unifiedY;
}

int Player::getSpeed() {
    return playerSpeed;
}
