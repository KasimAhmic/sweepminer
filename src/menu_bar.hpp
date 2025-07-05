#pragma once

#include <SDL3/SDL_render.h>

#include "game.hpp"

class MenuBar {
public:
    explicit MenuBar(Game* game);

    void draw(SDL_Window* window, SDL_Renderer* renderer);

    [[nodiscard]] float getHeight() const { return this->height; }
    void setHeight(const float height) { this->height = height; }

private:
    Game* game;
    float height;
};
