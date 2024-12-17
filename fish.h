#ifndef FISH_H
#define FISH_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "camera.h"
#include "env.h"

class Fish {
private:
    const int VISUAL_RANGE = 100;
    const int PROTECTED_RANGE = 20;
    const float AVOIDANCE_FORCE = 0.05;
    const float MATCHING_FACTOR = 0.05;
    const float CENTERING_FACTOR = 0.005;
    const float TURN_FACTOR = 0.3;
    const float MAX_SPEED = 13;
    const float MIN_SPEED = 0.6;
    const float BIASVALUE = 0.001;

    float x, y;
    float vx, vy;
    std::vector<Fish> &school;
    int id;
    SDL_Texture* texture;
    int width, height;
    int biasdir = 1;

    void checkNeighborhood(Fish &other, float &xpos_avg, float &ypos_avg, float &xvel_avg, float &yvel_avg, int &neighboring_boids, float &
                           close_dx, float &close_dy);

public:
    Fish(const int x, const int y, const float vx, const float vy, std::vector<Fish> &school, const int id, const int width, const int height, SDL_Renderer* renderer, int biasdir, SDL_Texture* texture);
    ~Fish() = default;

    float getX() const { return x; };
    float getY() const { return y; };
    float getVx() const { return vx; };
    float getVy() const { return vy; };
    int getId() const { return id; };

    void draw(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, float vx, float vy);
    void cycle(int iter);

    bool isInView(Fish& other);
    bool isClose(Fish& other);

    static bool SortByX(const Fish &a, const Fish &b) {
        return a.getX() < b.getX();
    }

    static void insertionSort(std::vector<Fish>& school) ;

    // Copy constructor
    Fish(const Fish& other);

    // Copy assignment operator
    Fish& operator=(const Fish& other);
};

#endif //FISH_H