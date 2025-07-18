#pragma once

#include "SDL3/SDL.h"

struct AppContext {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};
