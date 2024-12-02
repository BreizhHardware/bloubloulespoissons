#include <iostream>
#include "env.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#ifndef MENU_H
#define MENU_H

struct Button {
    SDL_Rect rect;
    SDL_Texture* txt;
    SDL_Color fontColor;
    SDL_Color bgColor;
};


class Menu {
    private:
        SDL_Texture* backgroundTxt = nullptr;
        SDL_Rect backgroundRect = {0, 0, windowWidth, windowHeight};
        std::string menuTitle = "BloubBloub les poissons";
        std::vector<Button> buttons;
        SDL_Renderer* renderer;


    public:
        Menu(SDL_Renderer* renderer){
            this->renderer = renderer;
            SDL_Surface* backgroundSurface = IMG_Load("../img/menu/background.png");
            if (backgroundSurface == nullptr) {
                std::cerr << "Erreur de chargement de l'image du menu: " << IMG_GetError() << std::endl;
            } else {
                backgroundTxt = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
                SDL_FreeSurface(backgroundSurface);
            }
        };

        void draw(SDL_Renderer* renderer);

        void addButton(int x, int y, int w, int h, std::string text, int size);
};

#endif