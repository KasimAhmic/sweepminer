#pragma once

#include <utility>

#include <SDL3/SDL.h>

namespace Events {
    inline bool initialized = false;

    inline uint32_t REVEAL_CELL = 0;
    inline uint32_t WIN_GAME = 0;
    inline uint32_t LOSE_GAME = 0;
    inline uint32_t MENU_CLICK = 0;

    inline void init() {
        if (initialized) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Attempted to initialize events more than once");
            return;
        }

        REVEAL_CELL = SDL_RegisterEvents(1);
        WIN_GAME = SDL_RegisterEvents(1);
        LOSE_GAME = SDL_RegisterEvents(1);
        MENU_CLICK = SDL_RegisterEvents(1);

        initialized = true;
    }

    inline SDL_Event CreateSweepMinerEvent(const uint32_t eventType, const int32_t data) {
        SDL_Event event{};
        event.type = eventType;
        event.user.type = eventType;
        event.user.code = data;
        return event;
    }

    inline SDL_Event CreateRevealCellEvent(const uint8_t row, const uint8_t column) {
        return CreateSweepMinerEvent(Events::REVEAL_CELL, row << 8 | column);
    }

    inline std::pair<uint8_t, uint8_t> GetRevealedCell(const SDL_Event &event) {
        const uint8_t row = event.user.code >> 8;
        const uint8_t column = event.user.code & 0xFF;
        return {row, column};
    }
}
