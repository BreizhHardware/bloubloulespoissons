//
// Created by BreizhHardware on 13/09/2024.
//

#ifndef DECORS_H
#define DECORS_H
#include <SDL_render.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "camera.h"

class Rock {
public:
    Rock(int x, int y, int size, Uint8 r, Uint8 g, Uint8 b) : x(x), y(y), size(size), r(r), g(g), b(b) {};
    void draw(SDL_Renderer* renderer) const;
    int x;
    int y;
private:
    int size;
    int r;
    int g;
    int b;
};

class Kelp {
public:
    Kelp(int x, int y, int height, Uint8 r, Uint8 g, Uint8 b) : x(x), y(y), size(height), r(r), g(g), b(b) {};
    void draw(SDL_Renderer* renderer) const;
    int x;
    int y;
private:

    int size;
    int r;
    int g;
    int b;
};

void generateProceduralDecorations(std::vector<Kelp>& kelps, std::vector<Rock>& rocks, int envHeight, int envWidth);


#endif //DECORS_H
