//
// Created by BreizhHardware on 13/09/2024.
//

#ifndef DECORS_H
#define DECORS_H
#include <SDL_render.h>
#include <ctime>
#include <stdlib.h>
#include <vector>

class Rock {
public:
    Rock(int x, int y, int size, int r, int g, int b) : x(x), y(y), size(size), r(r), g(g), b(b) {}
    void draw(SDL_Renderer* renderer);
private:
    int x;
    int y;
    int size;
    int r;
    int g;
    int b;
};

class Reef {
public:
    Reef(int x, int y);
    void draw(SDL_Renderer* renderer);

private:
    int x;
    int y;
    int size;
    std::vector<Rock> rocks;
};

class Kelp {
public:
    Kelp(int x, int y, int size, int r, int g, int b) : x(x), y(y), size(size), r(r), g(g), b(b) {}
    void draw(SDL_Renderer* renderer);

private:
    int x;
    int y;
    int size;
    int r;
    int g;
    int b;
};



#endif //DECORS_H
