#pragma once

#include "win.hpp"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"

struct AppContext {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    MIX_Mixer* mixer{};
    MIX_Track* audioTrack{};
    SDL_AppResult appQuit = SDL_APP_CONTINUE;

#ifdef _WIN32
    HWND windowHandle{};
#endif
};
