//
// Created by BreizhHardware on 05/11/2024.
//

#ifndef CAMERA_H
#define CAMERA_H


class Camera {
    public:
        static Camera& getInstance() {
            static Camera instance;
            return instance;
        }
        int getX() const { return x; }
        int getY() const { return y; }
        void setPosition(int newX, int newY) { x = newX; y = newY; }
        void move(int dx, int dy) { x += dx; y += dy; }
    private:
        Camera(): x(0), y(0) {}
        int x, y;
};



#endif //CAMERA_H
