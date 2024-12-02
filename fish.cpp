#include "fish.h"

Fish::Fish(const int x, const int y, const float vx, const float vy, std::vector<Fish> &school, const int id, const int width, const int height, SDL_Renderer* renderer, int biasdir, SDL_Texture* texture)
    : x(x), y(y), vx(vx), vy(vy), school(school), id(id), width(width), height(height), biasdir(biasdir), texture(texture) {
    // Constructor implementation
}

Fish::Fish(const Fish& other)
    : x(other.x), y(other.y), vx(other.vx), vy(other.vy), school(other.school), id(other.id), texture(other.texture), width(other.width), height(other.height), biasdir(other.biasdir) {
    // Additional initialization if needed
}

Fish& Fish::operator=(const Fish& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        vx = other.vx;
        vy = other.vy;
        school = other.school;
        id = other.id;
        texture = other.texture;
        width = other.width;
        height = other.height;
        biasdir = other.biasdir;
    }
    return *this;
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
    Camera &camera = Camera::getInstance();
    int cameraX = camera.getX();
    int cameraY = camera.getY();

    SDL_Rect rect = {static_cast<int>(x) - cameraX, static_cast<int>(y) - cameraY, width, height};
    float angle = atan2(vy, vx) * 180 / M_PI; // Convert angle to degrees

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        SDL_RenderFillRect(renderer, &rect);
    }
    //drawArrow(renderer, x + width / 2, y + height / 2, vx, vy);
}

bool Fish::isInView(Fish &other) {
    return (other.getX() >= x - VISUAL_RANGE && other.getX() <= x + VISUAL_RANGE &&
            other.getY() >= y - VISUAL_RANGE && other.getY() <= y + VISUAL_RANGE);
}

bool Fish::isClose(Fish &other) {
    return (other.getX() >= x - PROTECTED_RANGE && other.getX() <= x + PROTECTED_RANGE &&
            other.getY() >= y - PROTECTED_RANGE && other.getY() <= y + PROTECTED_RANGE);
}

void Fish::cycle(int iter) {
    int neighboring_boids = 0;
    float xvel_avg = 0, yvel_avg = 0, xpos_avg = 0, ypos_avg = 0, close_dx = 0, close_dy = 0;
    for (int i = 0; i < school.size(); i++) {
        if (school[i].getId() != id) {
            if (school[i].getX() - VISUAL_RANGE > x + VISUAL_RANGE) { break; }
            if (isInView(school[i])) {
                if (isClose(school[i])) {
                    close_dx += x - school[i].getX();
                    close_dy += y - school[i].getY();
                }
                xpos_avg += school[i].getX();
                ypos_avg += school[i].getY();
                xvel_avg += school[i].getVx();
                yvel_avg += school[i].getVy();
                neighboring_boids++;
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

    if (y <= 0) {
        vy += TURN_FACTOR;
    }
    if (y >= (ENV_WIDTH - MARGIN_HEIGHT)) {
        vy -= TURN_FACTOR;
    }
    if (x <= 0) {
        vx += TURN_FACTOR;
    }
    if (x >= (ENV_WIDTH - MARGIN_WIDTH)) {
        vx -= TURN_FACTOR;
    }

    if (biasdir == 1) {
        vx = (1 - BIASVALUE) * vx + (BIASVALUE * 1);
    } else if (biasdir == 0) {
        vx = (1 - BIASVALUE) * vx + (BIASVALUE * -1);
    }

    float speed = sqrt(vx * vx + vy * vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    } else if (speed < MIN_SPEED) {
        vx = vx / speed * MIN_SPEED;
        vy = vy / speed * MIN_SPEED;
    }
    x += vx;
    y += vy;
}