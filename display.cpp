//
// Created by BreizhHardware on 08/12/2024.
//

#include "display.h"

double calculateDistance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

void displayNearbyPlayers(SDL_Renderer* renderer, TTF_Font* font, Player& currentPlayer, std::vector<Player>& players, double threshold) {
    int currentX = currentPlayer.getUnifiedX();
    int currentY = currentPlayer.getUnifiedY();
    int offsetY = 90;

    for (auto& player : players) {
        if (&player == &currentPlayer) continue; // Skip the current player

        int playerX = player.getUnifiedX();
        int playerY = player.getUnifiedY();
        double distance = calculateDistance(currentX, currentY, playerX, playerY);
        if (distance <= threshold) {
            std::string nearbyPlayerText = "Nearby Player: (" + std::to_string(playerX) + ", " + std::to_string(playerY) + ")";
            SDL_Color textColor = {0, 255, 0};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, nearbyPlayerText.c_str(), textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {10, offsetY, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            // Affiche la 1ère image de playerV2-full.png
            SDL_Rect playerRect = {69, 73, 112, 117};
            SDL_Rect playerPosForRender = {playerX, playerY, 112, 117};
            SDL_RenderCopy(renderer, player.getTexture(), &playerRect, &playerPosForRender);

            offsetY += 20;
        }
    }
}