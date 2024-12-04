#include <iostream>
#include "env.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <functional>

#ifndef MENU_H
#define MENU_H

struct Button {
    SDL_Rect rect;
    SDL_Texture* txt;
    SDL_Color fontColor;
    SDL_Color bgColor;
    SDL_Rect txtRect;
    std::function<void()> callback;
    bool isTextInput = false;
    std::string inputText;
};

struct Text {
    SDL_Rect rect;
    SDL_Texture* txt;
    SDL_Color fontColor;
    SDL_Rect txtRect;
};


struct Page {
    std::string title;
    std::vector<Button> buttons;
    std::vector<Text> texts;
};

class Menu {
    private:
        SDL_Texture* backgroundTxt = nullptr;
        SDL_Rect backgroundRect = {0, 0, windowWidth, windowHeight};
        std::string menuTitle = "BloubBloub les poissons";
        SDL_Renderer* renderer;
        bool shown = true;
        std::vector<Page> pages;
        int currentPage = -1;
        int activeTextInputIndex = -1;


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

        void handleClickedButton();

        void hide();

        void show();

        bool isShown();

        void draw(SDL_Renderer* renderer);

        void changePage(std::string title);

        void addButton(std::string page, int x, int y, int w, int h, std::string text, int size, std::function<void()> callback, bool isTextInput = false);

        void addText(std::string page, int x, int y, int w, int h, std::string text, int size);

        void addPage(std::string title);
};

#endif