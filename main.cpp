#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>

// Function to draw a gradient background
void drawGradientBackground(SDL_Renderer* renderer) {
    for (int y = 0; y < ENV_HEIGHT; ++y) {
        SDL_SetRenderDrawColor(renderer, y * 255 / ENV_HEIGHT, y * 255 / ENV_HEIGHT, 255, 255);
        SDL_RenderLine(renderer, 0, y, ENV_WIDTH, y);
    }
}

// Function to draw a grid background
void drawGridBackground(SDL_Renderer* renderer) {
    for (int y = 0; y < ENV_HEIGHT; y += 50) {
        SDL_RenderLine(renderer, 0, y, ENV_WIDTH, y);
        std::string yText = std::to_string(y);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, yText.c_str(), yText.length(), {255, 255, 255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {0, y, textSurface->w, textSurface->h};
        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
        SDL_DestroySurface(textSurface);
    }
    for (int x = 0; x < ENV_WIDTH; x += 50) {
        SDL_RenderLine(renderer, x, 0, x, ENV_HEIGHT);
        std::string xText = std::to_string(x);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, xText.c_str(), xText.length(), {255, 255, 255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {x, 0, textSurface->w, textSurface->h};
        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
        SDL_DestroySurface(textSurface);
    }
}

// Function to display FPS
void displayFPS(SDL_Renderer* renderer, TTF_Font* font, int fps) {
    SDL_Color color = {255, 255, 255};
    std::string fpsText = "FPS: " + std::to_string(fps);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, fpsText.c_str(), fpsText.length(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    SDL_DestroySurface(textSurface);
}

// Function to display player coordinates
void displayPlayerCoord(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color textColor = {255, 255, 255};
    std::string coordText = "X: " + std::to_string(playerX);
    std::string coordText2 = "Y: " + std::to_string(playerY);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, coordText.c_str(), coordText.length(), textColor);
    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, coordText2.c_str(), coordText2.length(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
    SDL_Rect textRect = {10, 30, textSurface->w, textSurface->h};
    SDL_Rect textRect2 = {10, 50, textSurface2->w, textSurface2->h};
    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    SDL_RenderTexture(renderer, textTexture2, nullptr, &textRect2);
    SDL_DestroySurface(textSurface);
    SDL_DestroySurface(textSurface2);
}

// Function to initialize SDL
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur d'initialisation de SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "Erreur d'initialisation de SDL_image: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "Erreur d'initialisation de SDL_ttf: " << SDL_GetError() << std::endl;
        return false;
    }
    font = TTF_OpenFont("path/to/font.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Erreur de chargement de la police: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Erreur de création de la fenêtre: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_Surface* iconSurface = IMG_Load("path/to/icon.png");
    if (iconSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'icône: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_SetWindowIcon(window, iconSurface);
    SDL_DestroySurface(iconSurface);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Erreur de création du renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_Surface* schoolSurface = IMG_Load("path/to/school.png");
    if (schoolSurface == nullptr) {
        std::cerr << "Erreur de chargement de l'image de fond: " << SDL_GetError() << std::endl;
        return false;
    }
    schoolTexture = SDL_CreateTextureFromSurface(renderer, schoolSurface);
    SDL_DestroySurface(schoolSurface);
    return true;
}

// Function to handle events
void handleEvents(int& playerX, int& playerY, const int playerSpeed) {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
    }
    Camera& camera = Camera::getInstance();
    if (keystate[SDL_SCANCODE_W]) {
        if (playerY > 0) {
            playerY -= playerSpeed;
        }
        if (camera.getY() > 0 && playerY < playerBaseY) {
            camera.move(0, -playerSpeed);
        }
    }
    if (keystate[SDL_SCANCODE_S]) {
        if (playerY < ENV_HEIGHT - 75) {
            playerY += playerSpeed;
        }
        if (camera.getY() < ENV_HEIGHT - windowHeight && playerY > playerBaseY) {
            camera.move(0, playerSpeed);
        }
    }
    if (keystate[SDL_SCANCODE_A]) {
        if(camera.getX() > -playerBaseX) {
            camera.move(-playerSpeed, 0);
        }
    }
    if (keystate[SDL_SCANCODE_D]) {
        if(camera.getX() < ENV_WIDTH - windowWidth) {
            camera.move(playerSpeed, 0);
        }
    }
    if (keystate[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }
    if (playerX < 0) {
        playerX = 0;
    } else if (playerX > ENV_WIDTH) {
        playerX = ENV_WIDTH;
    }
    if (playerY < 0) {
        playerY = 0;
    } else if (playerY > ENV_HEIGHT) {
        playerY = ENV_HEIGHT;
    }
}

// Function to render the scene
void renderScene(int playerX, int playerY, int *fig) {
    static Uint32 lastTime = 0;
    static int frameCount = 0;
    static int fps = 0;
    Uint32 currentTime = SDL_GetTicks();
    frameCount++;
    if (currentTime - lastTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        lastTime = currentTime;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Camera& camera = Camera::getInstance();
    SDL_FRect backgroundRect = { -camera.getX(), -camera.getY(), ENV_WIDTH, ENV_HEIGHT };
    SDL_RenderTexture(renderer, backgroundTexture, nullptr, &backgroundRect);
    rock.draw(renderer);
    kelp.draw(renderer);
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& fish : school) {
        fish.draw(renderer);
    }
    SDL_FRect playerRect = {0, 0, 513, 600};
    if (*fig < 5 ) {
        playerRect = {46, 26, 442, 541};
    } else if (*fig < 8) {
        playerRect = {560, 23, 426, 536};
    }else if (*fig < 11) {
        playerRect = {986, 23, 469, 530};
    }else if (*fig < 14) {
        playerRect = {1436, 23, 465, 520};
    }else{
        *fig = 0;
    }
    *fig += 1;
    SDL_FRect playerPos = {playerX, playerY, 75, 75};
    SDL_RenderTextureRotated(renderer, playerTexture, &playerRect, &playerPos, 0, nullptr, SDL_FLIP_NONE);
    displayFPS(renderer, font, fps);
    displayPlayerCoord(renderer, font);
    SDL_RenderPresent(renderer);
}