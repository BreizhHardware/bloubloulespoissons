#ifndef FISH_H
#define FISH_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <iostream>
#include <string>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1000;
const int ENV_WIDTH = 1600;
const int ENV_HEIGHT = 1000;

#include <vector>

/*
class Fish {
public:
    Fish(int x, int y, int vx, int vy, int width, int height, SDL_Renderer* renderer, SDL_Texture* texture);
    ~Fish();

    void move();
    void draw(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, int vx, int vy);

private:
    int x, y;
    int vx, vy;
    int width, height;

};
*/

class Fish {
private:
    const int VISUAL_RANGE = 40;
    const int PROTECTED_RANGE = 8;
    const float AVOIDANCE_FORCE = 0.05;
    const float MATCHING_FACTOR = 0.05;
    const float CENTERING_FACTOR = 0.005;
    const float MAX_SPEED = 0.13;
    const float MIN_SPEED = 0.06;
    int x, y;
    float vx, vy;
    std::vector<Fish> &school;
    int id;
    SDL_Texture* texture;
    int cycle_count = 0;
    int width, height;

public:
    Fish(const int x, const int y, const int vx, const int vy,std::vector<Fish> &school, const int id,const int width,const int height, SDL_Texture* texture, SDL_Renderer* renderer): x(x), y(y), vx(vx), vy(vy), school(school), id(id), width(width),height(height), texture(texture) {}
    ~Fish() = default;

    int getX() const { return x; };
    int getY() const { return y; };
    float getVx() const { return vx; };
    float getVy() const { return vy; };
    int getId() const { return id; };


    void draw(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, int vx, int vy);
    void move();
    void cycle();

};

#endif //FISH_H