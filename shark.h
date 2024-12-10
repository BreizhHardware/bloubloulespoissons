//
// Created by cfouche on 09/12/24.
//

#ifndef SHARK_H
#define SHARK_H

#include <vector>
#include "player.h"
#include <SDL2/SDL.h>

class Shark {
private:
    const int VISUAL_RANGE = 500;
    const float AVOIDANCE_FORCE = 0.05;
    const float MATCHING_FACTOR = 0.05;
    const float CENTERING_FACTOR = 0.005;
    const float TURN_FACTOR = 0.3;
    const float MAX_SPEED = 4;
    const float MIN_SPEED = 0.6;
    const float BIASVALUE = 0.001;

    float x, y;
    float vx, vy;
    int id;
    SDL_Texture* texture;
    int width, height;
    std::vector<Player> &players_list;


public:
    Shark(int x, int y, float vx, float vy, int id, int width, int height, SDL_Renderer *renderer,std::vector<Player> &players_list);
    ~Shark() = default;

    float getX() const { return x; };
    float getY() const { return y; };
    float getVx() const { return vx; };
    float getVy() const { return vy; };




    void draw(SDL_Renderer* renderer);
    void cycle();

    bool isInView(Player& player);
    void checkNeighborhood(Player& player, float &xpos_avg, float &ypos_avg, float &xvel_avg, float &yvel_avg, int &neighboring_player);

};

#endif //SHARK_H
