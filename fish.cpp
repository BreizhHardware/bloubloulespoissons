#include "fish.h"

void Fish::move() {
    Fish::cycle();
    /*
    if (x < 0 || x >= ENV_WIDTH) {
        vx = -vx;
    }
    if (y < 0 || y >= ENV_HEIGHT) {
        vy = -vy;
    }
    */
}


void Fish::drawArrow(SDL_Renderer *renderer, int x, int y, float vx, float vy) {
    const int arrowLength = 30; // Augmenter la longueur de la flèche
    const int arrowWidth = 10; // Garder la largeur de la flèche constante
    SDL_Point points[4];

    if (vx == 0 && vy == 0) return;

    float angle = atan2(vy, vx);
    points[0] = {x + static_cast<int>(arrowLength * cos(angle)), y + static_cast<int>(arrowLength * sin(angle))};
    points[1] = {
        x + static_cast<int>(arrowWidth * cos(angle + M_PI / 6)),
        y + static_cast<int>(arrowWidth * sin(angle + M_PI / 6))
    };
    points[2] = {
        x + static_cast<int>(arrowWidth * cos(angle - M_PI / 6)),
        y + static_cast<int>(arrowWidth * sin(angle - M_PI / 6))
    };
    points[3] = points[0];

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
    SDL_RenderDrawLines(renderer, points, 4);
}

void Fish::draw(SDL_Renderer *renderer) {
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), width, height};
    float angle = atan2(vy, vx) * 180 / M_PI; // Convert angle to degrees

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        SDL_RenderFillRect(renderer, &rect);
    }
    //drawArrow(renderer, x + width / 2, y + height / 2, vx, vy);
}

void Fish::cycle() {
    //std::cout << "Cycle " << cycle_count << " pour le poisson " << id << std::endl;
    //std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
    //std::cout << "Vitesse: (" << vx << ", " << vy << ")" << std::endl;
    int neighboring_boids = 0;
    float xvel_avg = 0, yvel_avg = 0, xpos_avg = 0, ypos_avg = 0, close_dx = 0, close_dy = 0;
    for (auto &schoolIt: school) {
        const float other_x = schoolIt.getX();
        const float other_y = schoolIt.getY();
        const float other_vx = schoolIt.getVx();
        const float other_vy = schoolIt.getVy();
        if (schoolIt.getId() != id) {
            float dx = x - other_x;
            float dy = y - other_y;
            if (abs(dx) < VISUAL_RANGE && abs(dy) < VISUAL_RANGE) {
                if ((dx * dx + dy * dy) < PROTECTED_RANGE * PROTECTED_RANGE) {
                    close_dx += x - other_x;
                    close_dy += y - other_y;
                } else if ((dx * dx + dy * dy) < VISUAL_RANGE * VISUAL_RANGE) {
                    xpos_avg += other_x;
                    ypos_avg += other_y;
                    xvel_avg += other_vx;
                    yvel_avg += other_vy;
                    neighboring_boids++;
                }
            }
        }
    }
    if (neighboring_boids > 0) {
        xpos_avg /= neighboring_boids;
        ypos_avg /= neighboring_boids;
        xvel_avg /= neighboring_boids;
        yvel_avg /= neighboring_boids;
        vx += (xpos_avg - x) * CENTERING_FACTOR + (xvel_avg - vx) * MATCHING_FACTOR;
        vy += (ypos_avg - y) * CENTERING_FACTOR + (yvel_avg - vy) * MATCHING_FACTOR;
    }
    vx += close_dx * AVOIDANCE_FORCE;
    vy += close_dy * AVOIDANCE_FORCE;

    if ( y <= 0) {
        vy += TURN_FACTOR;
    }
    if ( y >= ENV_WIDTH - MARGIN_HEIGHT) {
        vy -= TURN_FACTOR;
    }
    if ( x <= 0) {
        vx += TURN_FACTOR;
    }
    if ( x >= ENV_WIDTH - MARGIN_WIDTH) {
        vx -= TURN_FACTOR;
    }

    if (biasdir == 1) {
        vx = (1 - BIASVALUE) * vx + (BIASVALUE * 1);
    }else if (biasdir == 0){
        vx = (1 - BIASVALUE) * vx + (BIASVALUE * -1);
    }

    if (float speed = sqrt(vx * vx + vy * vy); speed > MAX_SPEED) {
        vx = vx * MAX_SPEED / speed;
        vy = vy * MAX_SPEED / speed;
    } else if (speed < MIN_SPEED) {
        vx = vx * MIN_SPEED / speed;
        vy = vy * MIN_SPEED / speed;
    }
    x += vx;
    y += vy;
    cycle_count++;
    //std::cout << "Updated Position: (" << x << ", " << y << ")" << std::endl;
    //std::cout << "Updated Vitesse: (" << vx << ", " << vy << ")" << std::endl;
}
