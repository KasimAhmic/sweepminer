#include "box.hpp"

Box::Box(Context* context,
         const SDL_FRect& rect,
         const float borderWidth,
         const Color& borderHighlightColor,
         const Color& borderShadowColor,
         const Color& backgroundColor)
    : UiComponent(context, rect),
      borderWidth(borderWidth * SDL_GetWindowDisplayScale(context->getWindow())),
      borderPrimaryColor(borderHighlightColor),
      borderSecondaryColor(borderShadowColor),
      backgroundColor(backgroundColor) {}

Box::~Box() = default;

void Box::render() {
    const auto& renderer = this->getContext().getRenderer();

    const float s = this->getContext().getScale();
    const float x = this->getRect().x;
    const float y = this->getRect().y;
    const float w = this->getRect().w;
    const float h = this->getRect().h;
    const float b = this->getBorderWidth() * s;
    const SDL_FColor primaryColor = this->getBorderPrimaryColor().asFloat();
    const SDL_FColor secondaryColor = this->getBorderSecondaryColor().asFloat();

    const SDL_Vertex borderVertices[6] = {
        SDL_Vertex{ .position = SDL_FPoint{ .x = x,     .y = y     }, .color = primaryColor },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x,     .y = y + h }, .color = primaryColor },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x + w, .y = y     }, .color = primaryColor },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x + w, .y = y + h }, .color = secondaryColor },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x + w, .y = y     }, .color = secondaryColor },
        SDL_Vertex{ .position = SDL_FPoint{ .x = x,     .y = y + h }, .color = secondaryColor },
    };

    SDL_RenderGeometry(renderer, nullptr, borderVertices, 6, nullptr, 0);

    if (w > h) {
        const SDL_Vertex cornerFixVertices[6] = {
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w,         .y = y         }, .color = primaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w - h + b, .y = y         }, .color = primaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w - h + b, .y = y + h - b }, .color = primaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x,         .y = y + h         }, .color = secondaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + h - b, .y = y + h         }, .color = secondaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + h - b, .y = y + b         }, .color = secondaryColor },
        };

        SDL_RenderGeometry(renderer, nullptr, cornerFixVertices, 6, nullptr, 0);
    }

    if (h > w) {
        const SDL_Vertex cornerFixVertices[6] = {
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w, .y = y + w - b         }, .color = secondaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + b, .y = y + w - b         }, .color = secondaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w, .y = y                 }, .color = secondaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x,         .y = y + h - w + b }, .color = primaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x + w - b, .y = y + h - w + b }, .color = primaryColor },
            SDL_Vertex{ .position = SDL_FPoint{ .x = x,         .y = y + h         }, .color = primaryColor },
        };

        SDL_RenderGeometry(renderer, nullptr, cornerFixVertices, 6, nullptr, 0);
    }

    SDL_SetRenderDrawColor(renderer, SpreadColorInt(this->getBackgroundColor()));

    const SDL_FRect rect = {
        .x = x + b,
        .y = y + b,
        .w = w - b * 2,
        .h = h - b * 2
    };

    SDL_RenderFillRect(renderer, &rect);
}
