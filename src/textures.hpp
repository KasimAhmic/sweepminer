#pragma once

#include <stdexcept>

#include "SDL3/SDL.h"

namespace TextureOffset {
    constexpr SDL_FRect NONE =           { 00, 00, 16, 16 };
    constexpr SDL_FRect FLAG =           { 16, 00, 16, 16 };
    constexpr SDL_FRect QUESTION_MARK =  { 32, 00, 16, 16 };

    constexpr SDL_FRect MINE =           { 00, 16, 16, 16 };
    constexpr SDL_FRect MINE_FLAGGED =   { 16, 16, 16, 16 };
    constexpr SDL_FRect MINE_DETONATED = { 32, 16, 16, 16 };

    constexpr SDL_FRect COUNT_ONE =      { 00, 32, 16, 16 };
    constexpr SDL_FRect COUNT_TWO =      { 16, 32, 16, 16 };
    constexpr SDL_FRect COUNT_THREE =    { 32, 32, 16, 16 };
    constexpr SDL_FRect COUNT_FOUR =     { 00, 48, 16, 16 };
    constexpr SDL_FRect COUNT_FIVE =     { 16, 48, 16, 16 };
    constexpr SDL_FRect COUNT_SIX =      { 32, 48, 16, 16 };
    constexpr SDL_FRect COUNT_SEVEN =    { 00, 64, 16, 16 };
    constexpr SDL_FRect COUNT_EIGHT =    { 16, 64, 16, 16 };

    constexpr SDL_FRect NUMBER_ZERO =    {  00, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_ONE =     {  13, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_TWO =     {  26, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_THREE =   {  39, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_FOUR =    {  52, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_FIVE =    {  65, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_SIX =     {  78, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_SEVEN =   {  91, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_EIGHT =   { 104, 00, 13, 23 };
    constexpr SDL_FRect NUMBER_NINE =    { 117, 00, 13, 23 };

    inline const SDL_FRect* getNumberTextureOffset(const uint8_t number) {
        switch (number) {
            case 0: return &TextureOffset::NUMBER_ZERO;
            case 1: return &TextureOffset::NUMBER_ONE;
            case 2: return &TextureOffset::NUMBER_TWO;
            case 3: return &TextureOffset::NUMBER_THREE;
            case 4: return &TextureOffset::NUMBER_FOUR;
            case 5: return &TextureOffset::NUMBER_FIVE;
            case 6: return &TextureOffset::NUMBER_SIX;
            case 7: return &TextureOffset::NUMBER_SEVEN;
            case 8: return &TextureOffset::NUMBER_EIGHT;
            case 9: return &TextureOffset::NUMBER_NINE;
            default: throw std::out_of_range("Number must be between 0 and 9");
        }
    }

    inline const SDL_FRect* getCountTextureOffset(const uint8_t surroundingMines) {
        switch (surroundingMines) {
            case 1: return &TextureOffset::COUNT_ONE;
            case 2: return &TextureOffset::COUNT_TWO;
            case 3: return &TextureOffset::COUNT_THREE;
            case 4: return &TextureOffset::COUNT_FOUR;
            case 5: return &TextureOffset::COUNT_FIVE;
            case 6: return &TextureOffset::COUNT_SIX;
            case 7: return &TextureOffset::COUNT_SEVEN;
            case 8: return &TextureOffset::COUNT_EIGHT;
            default: throw std::out_of_range("Number must be between 0 and 8");
        }
    }
}
