#pragma once

#include <SDL3/SDL.h>

class Color {
public:
    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
        : intColor{r, g, b, a},
          floatColor{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f} {}

    [[nodiscard]] constexpr const SDL_Color& asInt() const { return this->intColor; }
    [[nodiscard]] constexpr const SDL_FColor& asFloat() const { return this->floatColor; }

private:
    SDL_Color intColor;
    SDL_FColor floatColor;
};

constexpr Color BACKGROUND_COLOR(192, 192, 192, 255);
constexpr Color HOVERED_COLOR(150, 150, 150, 255);
constexpr Color BORDER_HIGHLIGHT_COLOR(255, 255, 255, 255);
constexpr Color BORDER_SHADOW_COLOR(128, 128, 128, 255);
