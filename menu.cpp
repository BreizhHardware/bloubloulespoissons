#include "menu.h"

void Menu::draw(SDL_Renderer* renderer){
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTxt, nullptr, &backgroundRect);
    if(pages.size() > 0){
        if(currentPage == -1 && pages.size() > 0){
            currentPage = 0;
        }

        for(Text& text : pages[currentPage].texts){
            SDL_RenderCopy(renderer, text.txt, nullptr, &text.rect);
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool hover = false;
        for(Button& button : pages[currentPage].buttons){
            SDL_SetRenderDrawColor(renderer, button.bgColor.r, button.bgColor.g, button.bgColor.b, button.bgColor.a);
            SDL_RenderFillRect(renderer, &button.rect);
            SDL_RenderCopy(renderer, button.txt, nullptr, &button.txtRect);

            if (button.isTextInput) {
                // Afficher le texte saisi
                TTF_Font* font_txt = TTF_OpenFont("../fonts/arial.ttf", 24);
                SDL_Surface* textSurface = TTF_RenderText_Solid(font_txt, button.inputText.c_str(), button.fontColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect inputRect = {button.rect.x + 5, button.rect.y + 5, button.rect.w - 10, button.rect.h - 10};
                SDL_RenderCopy(renderer, textTexture, nullptr, &inputRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
                TTF_CloseFont(font_txt);
            }

            if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                hover = true;
            }
        }

        if (hover) {
            SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
            SDL_SetCursor(cursor);
        } else {
            SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
            SDL_SetCursor(cursor);
        }
    }

    SDL_RenderPresent(renderer);
}

void Menu::handleClickedButton(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            activeTextInputIndex = -1;
            for (int i = 0; i < pages[currentPage].buttons.size(); ++i) {
                Button& button = pages[currentPage].buttons[i];
                if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                    mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                    if (button.isTextInput) {
                        // Activer le champ de texte
                        SDL_StartTextInput();
                        activeTextInputIndex = i; // Mettre à jour l'élément de texte actif
                    } else {
                        button.callback();
                    }
                }
            }
        }else if (event.type == SDL_TEXTINPUT) {
            if (activeTextInputIndex != -1) {
                Button& button = pages[currentPage].buttons[activeTextInputIndex];
                if (button.isTextInput) {
                    button.inputText += event.text.text;
                }
            }
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE) {
            if (activeTextInputIndex != -1) {
                Button& button = pages[currentPage].buttons[activeTextInputIndex];
                if (button.isTextInput && !button.inputText.empty()) {
                    button.inputText.pop_back();
                }
            }
        }
    }
}

void Menu::hide(){
    shown = false;
}

void Menu::show(){
    shown = true;
}

bool Menu::isShown(){
    return shown;
}

void Menu::changePage(std::string title){
    for(int i = 0; i < pages.size(); i++){
        if(pages[i].title == title){
            currentPage = i;
        }
    }
}

void Menu::addButton(std::string page, int x, int y, int w, int h, std::string text, int size, std::function<void()> callback, bool isTextInput){
    for(Page& p : pages){
        if(p.title == page){
            TTF_Font* font_txt = TTF_OpenFont("../fonts/arial.ttf", size);
            if (font_txt == nullptr) {
                std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
            }
            Button button;
            button.rect = {x, y, w, h};
            int textWidth = w/2;
            int textHeight = h/1.5;
            button.txtRect = {x + (w/2) - (textWidth/2), y + (h/2) - (textHeight/2), textWidth, textHeight};
            button.fontColor = {255, 255, 255};
            button.bgColor = {0, 0, 255, 255};
            button.isTextInput = isTextInput;
            
            SDL_Surface* textSurface = TTF_RenderText_Solid(font_txt, text.c_str(), button.fontColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);
            button.txt = textTexture;
            button.callback = callback;
            p.buttons.push_back(button);
            TTF_CloseFont(font_txt);
        }
    }
}

void Menu::addText(std::string page, int x, int y, int w, int h, std::string text, int size){
    for(Page& p : pages){
        if(p.title == page){
            TTF_Font* font_txt = TTF_OpenFont("../fonts/arial.ttf", size);
            if (font_txt == nullptr) {
                std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
            }
            Text txt;
            txt.rect = {x, y, w, h};
            txt.fontColor = {255, 255, 255};
            txt.txtRect = {x, y, w, h};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font_txt, text.c_str(), txt.fontColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);
            txt.txt = textTexture;
            p.texts.push_back(txt);
            TTF_CloseFont(font_txt);
        }
    }
}


void Menu::addPage(std::string title){
    Page page;
    page.title = title;
    pages.push_back(page);
}


std::vector<Button> Menu::getButtons(){
    return pages[currentPage].buttons;
}