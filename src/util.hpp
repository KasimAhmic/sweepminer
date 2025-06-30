#pragma once

#include "SDL3/SDL.h"

#include "color.hpp"

inline void SetRenderDrawColor(SDL_Renderer* renderer, const Color& color) {

    SDL_SetRenderDrawColor(renderer, color.asInt().r, color.asInt().g, color.asInt().b, color.asInt().a);
}

inline void DrawBox(
    SDL_Renderer *renderer,
    const float x,
    const float y,
    const float width,
    const float height,
    const float borderWidth,
    const Color &backgroundColor,
    const Color &topLeftBorderColor,
    const Color &bottomRightBorderColor) {

    const SDL_Vertex topLeftBorders[3] = {
        SDL_Vertex{ SDL_FPoint{ .x = x, .y = y }, topLeftBorderColor.asFloat()},
        SDL_Vertex{ SDL_FPoint{ .x = x, .y = y + height }, topLeftBorderColor.asFloat() },
        SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y }, topLeftBorderColor.asFloat() },
    };

    const SDL_Vertex bottomRightBorders[3] = {
        SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y + height }, bottomRightBorderColor.asFloat() },
        SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y }, bottomRightBorderColor.asFloat() },
        SDL_Vertex{ SDL_FPoint{ .x = x, .y = y + height }, bottomRightBorderColor.asFloat() }
    };

    SDL_RenderGeometry(renderer, nullptr, topLeftBorders, 3, nullptr, 0);
    SDL_RenderGeometry(renderer, nullptr, bottomRightBorders, 3, nullptr, 0);

    if (width > height) {
        const SDL_Vertex bottomLeftTriangle[3] = {
            SDL_Vertex{ SDL_FPoint{ .x = x, .y = y + height }, bottomRightBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + height - borderWidth, .y = y + height }, bottomRightBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + height - borderWidth, .y = y + borderWidth }, bottomRightBorderColor.asFloat() }
        };

        const SDL_Vertex topRightTriangle[3] = {
            SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y }, topLeftBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + width - height + borderWidth, .y = y }, topLeftBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + width - height + borderWidth, .y = y + height - borderWidth }, topLeftBorderColor.asFloat() }
        };

        SDL_RenderGeometry(renderer, nullptr, topRightTriangle, 3, nullptr, 0);
        SDL_RenderGeometry(renderer, nullptr, bottomLeftTriangle, 3, nullptr, 0);
    }

    if (height > width) {
        const SDL_Vertex topRightTriangle[3] = {
            SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y + width - borderWidth }, bottomRightBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + borderWidth, .y = y + width - borderWidth }, bottomRightBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + width, .y = y }, bottomRightBorderColor.asFloat() }
        };

        const SDL_Vertex bottomLeftTriangle[3] = {
            SDL_Vertex{ SDL_FPoint{ .x = x, .y = y + height - width + borderWidth }, topLeftBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x + width - borderWidth, .y = y + height - width + borderWidth }, topLeftBorderColor.asFloat() },
            SDL_Vertex{ SDL_FPoint{ .x = x, .y = y + height }, topLeftBorderColor.asFloat() }
        };

        SDL_RenderGeometry(renderer, nullptr, topRightTriangle, 3, nullptr, 0);
        SDL_RenderGeometry(renderer, nullptr, bottomLeftTriangle, 3, nullptr, 0);
    }

    SDL_SetRenderDrawColor(renderer, backgroundColor.asInt().r, backgroundColor.asInt().g, backgroundColor.asInt().b, backgroundColor.asInt().a);

    const SDL_FRect cell = {
        x + borderWidth,
        y + borderWidth,
        width - borderWidth * 2,
        height - borderWidth * 2
    };

    SDL_RenderFillRect(renderer, &cell);
}
