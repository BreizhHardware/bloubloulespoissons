//
// Created by BreizhHardware on 13/09/2024.
//

#ifndef FISH_H
#define FISH_H
#include <SDL2/SDL.h>

class Fish {
public:
    int x, y;
    int vx, vy;
    int width, height;

    Fish(int x, int y, int vx, int vy, int width, int height) : x(x), y(y), vx(vx), vy(vy), width(width), height(height) {}

    void move();
    void draw(SDL_Renderer* renderer);
};



#endif //FISH_H
