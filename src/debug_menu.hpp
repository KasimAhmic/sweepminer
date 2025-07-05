#pragma once

#include <SDL3/SDL_render.h>

#include "game.hpp"

class DebugMenu {
public:
    explicit DebugMenu(Game* game);
    ~DebugMenu() = default;

    void draw(SDL_Renderer *renderer);
    void handleRevealAll();
    void handleRevealNumbers();
    void handleRevealMines();
    void updateHoveredCell();

private:
    Game* game;
    std::pair<uint8_t, uint8_t> cachedHoveredCell;
};
