//
// Created by BreizhHardware on 13/09/2024.
//

#ifndef FISH_H
#define FISH_H
#include <SDL2/SDL.h>
#include <math.h>

class Fish {
public:
    Fish(int x, int y, int vx, int vy, int width, int height) : x(x), y(y), vx(vx), vy(vy), width(width), height(height) {}

    void move();
    void draw(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, int vx, int vy);

private:
    int x, y;
    int vx, vy;
    int width, height;
};



#endif //FISH_H
