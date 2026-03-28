#include "box.hpp"

#include <array>

void Box::render() {
    const float x = this->rect.x;
    const float y = this->rect.y;
    const float w = this->rect.w;
    const float h = this->rect.h;

    const float x2 = x + w;
    const float y2 = y + h;

    const SDL_FColor background = this->getBackgroundColor().asFloat();
    const SDL_FColor borderHighlight = this->getBorderHighlightColor().asFloat();
    const SDL_FColor borderShadow = this->getBorderShadowColor().asFloat();
    const float border = this->getBorderWidth();

    const std::array topLeftBorderVertices = {
        SDL_Vertex{ .position = SDL_FPoint{ .x =  x, .y =  y }, .color = borderHighlight },
        SDL_Vertex{ .position = SDL_FPoint{ .x =  x, .y = y2 }, .color = borderHighlight },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x2, .y =  y }, .color = borderHighlight },
    };

    const std::array bottomRightBorderVertices = {
        SDL_Vertex{ .position = SDL_FPoint{ .x = x2, .y = y2 }, .color = borderShadow },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x2, .y =  y }, .color = borderShadow },
        SDL_Vertex{ .position = SDL_FPoint{ .x =  x, .y = y2 }, .color = borderShadow },
    };

    SDL_RenderGeometry(this->context.renderer, nullptr, topLeftBorderVertices.data(), 3, nullptr, 0);
    SDL_RenderGeometry(this->context.renderer, nullptr, bottomRightBorderVertices.data(), 3, nullptr, 0);

    if (w > h) {
        const std::array topRightTriangleVertices = {
            SDL_Vertex{ .position = SDL_FPoint{ .x = x2, .y = y }, .color = borderHighlight },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x2 - h + border, .y = y }, .color = borderHighlight },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x2 - h + border, .y = y2 - border }, .color = borderHighlight },
        };

        const std::array bottomLeftTriangleVertices = {
            SDL_Vertex{ .position = SDL_FPoint{ .x = x, .y = y2 }, .color = borderHighlight },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x2 - border, .y = y2 }, .color = borderHighlight },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x2 - border, .y = y + border }, .color = borderHighlight },
        };

        SDL_RenderGeometry(this->context.renderer, nullptr, topRightTriangleVertices.data(), topRightTriangleVertices.size(), nullptr, 0);
        SDL_RenderGeometry(this->context.renderer, nullptr, bottomLeftTriangleVertices.data(), bottomLeftTriangleVertices.size(), nullptr, 0);
    }

    if (h > w) {

    }

    SDL_SetRenderDrawColorFloat(this->context.renderer, background.r, background.g, background.b, background.a);

    const SDL_FRect cell = {
        .x = x + border,
        .y = y + border,
        .w = w - border * 2,
        .h = h - border * 2
    };

    SDL_RenderFillRect(this->context.renderer, &cell);
}
