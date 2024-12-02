#include "menu.h"

void Menu::draw(SDL_Renderer* renderer){
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTxt, nullptr, &backgroundRect);

    for(Button button : buttons){
        SDL_SetRenderDrawColor(renderer, button.bgColor.r, button.bgColor.g, button.bgColor.b, button.bgColor.a);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_RenderCopy(renderer, button.txt, nullptr, &button.rect);
    }
    
    // int mouseX, mouseY;
    // SDL_GetMouseState(&mouseX, &mouseY);
    // if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonWidth &&
    //     mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonHeight) {
    //     SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    //     SDL_SetCursor(cursor);
    // } else {
    //     SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    //     SDL_SetCursor(cursor);
    // }

    SDL_RenderPresent(renderer);
}

void Menu::addButton(int x, int y, int w, int h, std::string text, int size){
    

    TTF_Font* font_txt = TTF_OpenFont("../fonts/arial.ttf", size);
    if (font_txt == nullptr) {
        std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
    }
    Button button;
    button.rect = {x, y, w, h};
    button.fontColor = {255, 255, 255};
    button.bgColor = {0, 0, 255, 255};
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font_txt, text.c_str(), button.fontColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    button.txt = textTexture;
    buttons.push_back(button);
    TTF_CloseFont(font_txt);
}

