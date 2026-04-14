#pragma once

#include "SDL3/SDL.h"

// Expands the RGBA properties of a Color object to 4 distinct uint8_t arguments
#define SpreadColorInt(color) color.asInt().r, color.asInt().g, color.asInt().b, color.asInt().a

// Expands the RGBA properties of a Color object to 4 distinct float arguments
#define SpreadColorFloat(color) color.asFloat().r, color.asFloat().g, color.asFloat().b, color.asFloat().a

class Color {
public:
    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
        : intColor{r, g, b, a},
          floatColor{
              static_cast<float>(r) / 255.0f,
              static_cast<float>(g) / 255.0f,
              static_cast<float>(b) / 255.0f,
              static_cast<float>(a) / 255.0f
          } {
    }

    ~Color() = default;

    friend bool operator==(const Color& a, const Color& b) {
        const auto [aR, aG, aB, aA] = a.asInt();
        const auto [bR, bG, bB, bA] = b.asInt();

        return aR == bR && aG == bG && aB == bB;
    }

    [[nodiscard]] constexpr const SDL_Color& asInt() const { return this->intColor; }
    [[nodiscard]] constexpr const SDL_FColor& asFloat() const { return this->floatColor; }

private:
    SDL_Color intColor;
    SDL_FColor floatColor;
};

inline constexpr Color WHITE{255, 255, 255, 255};
inline constexpr Color BLACK{0, 0, 0, 255};
inline constexpr Color GREY{192, 192, 192, 255};
inline constexpr Color LIGHT_GREY{150, 150, 150, 255};
inline constexpr Color DARK_GREY{128, 128, 128, 255};
