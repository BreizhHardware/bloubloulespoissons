#ifndef FISH_H
#define FISH_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <iostream>
#include <string>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ENV_WIDTH = 1600;
const int ENV_HEIGHT = 1200;


class Fish {
public:
    Fish(int x, int y, int vx, int vy, int width, int height, SDL_Renderer* renderer, const char* imagePath);
    ~Fish();

    void move();
    void draw(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, int vx, int vy);

private:
    int x, y;
    int vx, vy;
    int width, height;
    SDL_Texture* texture;
};

#endif //FISH_H