#include "menu.h"


void drawRoundedRectWithGradient(SDL_Renderer* renderer, SDL_Rect rect, int radius, SDL_Color startColor, SDL_Color endColor, int gradientWidth) {
    for (int i = 0; i < gradientWidth; i++) {
        Uint8 r = startColor.r + i * (endColor.r - startColor.r) / gradientWidth;
        Uint8 g = startColor.g + i * (endColor.g - startColor.g) / gradientWidth;
        Uint8 b = startColor.b + i * (endColor.b - startColor.b) / gradientWidth;
        Uint8 a = startColor.a + i * (endColor.a - startColor.a) / gradientWidth;

        SDL_Color gradientColor = {r, g, b, a};

        SDL_SetRenderDrawColor(renderer, gradientColor.r, gradientColor.g, gradientColor.b, gradientColor.a);
        SDL_Rect gradientRect = {
            rect.x - i,
            rect.y - i,
            rect.w + 2 * i,
            rect.h + 2 * i
        };

        auto drawRoundedCorners = [&](int x, int y, int r) {
            for (int dy = -r; dy <= r; dy++) {
                for (int dx = -r; dx <= r; dx++) {
                    if (dx * dx + dy * dy <= r * r) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                    }
                }
            }
        };

        drawRoundedCorners(gradientRect.x + radius, gradientRect.y + radius, radius); 
        drawRoundedCorners(gradientRect.x + gradientRect.w - radius, gradientRect.y + radius, radius); 
        drawRoundedCorners(gradientRect.x + radius, gradientRect.y + gradientRect.h - radius, radius);
        drawRoundedCorners(gradientRect.x + gradientRect.w - radius, gradientRect.y + gradientRect.h - radius, radius);

        SDL_Rect top = {gradientRect.x + radius, gradientRect.y, gradientRect.w - 2 * radius, radius};
        SDL_Rect bottom = {gradientRect.x + radius, gradientRect.y + gradientRect.h - radius, gradientRect.w - 2 * radius, radius};
        SDL_Rect left = {gradientRect.x, gradientRect.y + radius, radius, gradientRect.h - 2 * radius};
        SDL_Rect right = {gradientRect.x + gradientRect.w - radius, gradientRect.y + radius, radius, gradientRect.h - 2 * radius};
        SDL_Rect center = {gradientRect.x + radius, gradientRect.y + radius, gradientRect.w - 2 * radius, gradientRect.h - 2 * radius};

        SDL_RenderDrawRect(renderer, &top);
        SDL_RenderDrawRect(renderer, &bottom);
        SDL_RenderDrawRect(renderer, &left);
        SDL_RenderDrawRect(renderer, &right);
    }
}

void drawRoundedRect(SDL_Renderer* renderer, SDL_Rect rect, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    auto drawRoundedCorners = [&](int x, int y, int r) {
        for (int dy = -r; dy <= r; dy++) {
            for (int dx = -r; dx <= r; dx++) {
                if (dx * dx + dy * dy <= r * r) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    };

    drawRoundedCorners(rect.x + radius, rect.y + radius, radius);
    drawRoundedCorners(rect.x + rect.w - radius, rect.y + radius, radius);
    drawRoundedCorners(rect.x + radius, rect.y + rect.h - radius, radius);
    drawRoundedCorners(rect.x + rect.w - radius, rect.y + rect.h - radius, radius);

    SDL_Rect top = {rect.x + radius, rect.y, rect.w - 2 * radius, radius};
    SDL_Rect bottom = {rect.x + radius, rect.y + rect.h - radius, rect.w - 2 * radius, radius};
    SDL_Rect left = {rect.x, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_Rect right = {rect.x + rect.w - radius, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_Rect center = {rect.x + radius, rect.y + radius, rect.w - 2 * radius, rect.h - 2 * radius};

    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
    SDL_RenderFillRect(renderer, &center);
}



void Menu::draw(SDL_Renderer* renderer) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTxt, nullptr, &backgroundRect);

    if (pages.size() > 0) {
        if (currentPage == -1 && pages.size() > 0) {
            currentPage = 0;
        }

        for (ImagePage image : pages[currentPage].images) {
            SDL_RenderCopy(renderer, image.image, nullptr, &image.rect);
        }

        for (Text& text : pages[currentPage].texts) {
            SDL_RenderCopy(renderer, text.txt, nullptr, &text.rect);
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool hover = false;
        for (Button& button : pages[currentPage].buttons) {
            // Activer le blending pour l'arrière-plan du bouton
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, button.bgColor.r, button.bgColor.g, button.bgColor.b, button.bgColor.a);
            SDL_RenderFillRect(renderer, &button.rect);

            // Dessiner la bordure du bouton
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, button.borderColor.r, button.borderColor.g, button.borderColor.b, button.borderColor.a);
            SDL_Rect borderRect = {button.rect.x - button.borderWidth, button.rect.y - button.borderWidth, button.rect.w + 2 * button.borderWidth, button.rect.h + 2 * button.borderWidth};
            SDL_RenderDrawRect(renderer, &borderRect);

            SDL_RenderCopy(renderer, button.txt, nullptr, &button.txtRect);

            if (button.isTextInput) {
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



void Menu::handleClickedButton() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }

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
        } else if (event.type == SDL_TEXTINPUT) {
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

void Menu::addButton(std::string page, int x, int y, int w, int h, std::string text, int size, std::function<void()> callback, bool isTextInput) {
    for (Page& p : pages) {
        if (p.title == page) {
            TTF_Font* font_txt = TTF_OpenFont("../fonts/arial.ttf", size);
            if (font_txt == nullptr) {
                std::cerr << "Erreur de chargement de la police: " << TTF_GetError() << std::endl;
            }
            Button button;
            button.rect = {x, y, w, h};
            int textWidth = w / 2;
            int textHeight = h / 1.5;
            button.txtRect = {x + (w / 2) - (textWidth / 2), y + (h / 2) - (textHeight / 2), textWidth, textHeight};
            button.fontColor = {255, 255, 255};
            button.bgColor = {0, 0, 0, 75}; // Fond noir avec opacité de 13%
            button.borderColor = {40, 120, 122, 255}; // Couleur de la bordure
            button.borderWidth = 5; // Largeur de la bordure
            button.borderRadius = 17; // Rayon de la bordure
            button.isTextInput = isTextInput;

            SDL_Surface* textSurface = TTF_RenderText_Solid(font_txt, text.c_str(), button.fontColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);
            button.txt = textTexture;
            button.callback = callback;
            button.startGradientColor = {40, 120, 122, 255};
            button.endGradientColor = {55, 171, 189, 255};
            button.gradientWidth = 10;

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

void Menu::addImage(std::string page, int x, int y, int w, int h, std::string path) {
    for (Page& p : pages) {
        if (p.title == page) {
            SDL_Surface* imageSurface = IMG_Load(path.c_str());
            if (imageSurface == nullptr) {
                std::cerr << "Erreur de chargement de l'image: " << IMG_GetError() << std::endl;
                return;
            }
            SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
            SDL_FreeSurface(imageSurface);
            SDL_Rect imageRect = {x, y, w, h};

            p.images.push_back(ImagePage(imageTexture, imageRect));
        }
    }
}

void Menu::drawLost(SDL_Renderer* renderer) {
    // Charger l'image perdu.png
    SDL_Surface* lostSurface = IMG_Load("../img/perdu.png");
    if (lostSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image perdu.png: " << IMG_GetError() << std::endl;
        return;
    }
    SDL_Texture* lostTexture = SDL_CreateTextureFromSurface(renderer, lostSurface);
    SDL_FreeSurface(lostSurface);

    // Définir la transparence (alpha) de l'image
    SDL_SetTextureBlendMode(lostTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(lostTexture, 128); // 128 pour 50% de transparence

    // Obtenir les dimensions de la fenêtre
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Définir le rectangle de destination pour l'image
    SDL_Rect destRect = {0, 0, windowWidth, windowHeight};

    // Dessiner l'image
    SDL_RenderCopy(renderer, lostTexture, nullptr, &destRect);

    // Détruire la texture après utilisation
    SDL_DestroyTexture(lostTexture);
}