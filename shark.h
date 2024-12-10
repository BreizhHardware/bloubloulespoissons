//
// Created by cfouche on 09/12/24.
//

#ifndef SHARK_H
#define SHARK_H

#include <SDL2/SDL.h>

class Shark {
private:
    float x, y;
    float vx, vy;
    int id;
    SDL_Texture* texture;
    int width, height;

public:
    Shark(const int x, const int y, const float vx, const float vy, const int id, const int width, const int height, SDL_Renderer* renderer, SDL_Texture* texture);
    ~Shark() = default;

    float getX() const { return x; };
    float getY() const { return y; };
    float getVx() const { return vx; };
    float getVy() const { return vy; };



};

#endif //SHARK_H
