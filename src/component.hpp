#pragma once

#include "context.hpp"
#include "SDL3/SDL_rect.h"

class Component {
public:
    explicit Component(Context* context, const SDL_FRect& rect);
    virtual ~Component();

    virtual void render();

    [[nodiscard]] Context& getContext() const { return *this->context; }
    [[nodiscard]] const SDL_FRect& getRect() const { return this->rect; }
    [[nodiscard]] const SDL_FRect& getBounds() const { return this->bounds; }
    void setRect(const SDL_FRect& newRect) { this->rect = newRect; }

private:
    Context* context;
    /**
     * The rectangle representing the position and size of this component, scaled by the display scale factor. This is
     * the rectangle that should be used for rendering and hit detection.
     */
    SDL_FRect rect;

    /**
     * The rectangle representing the position and size of this component, not scaled by the display scale factor. This
     * is the rectangle that should be used for layout calculations, as it represents the "logical" size of the
     * component.
     */
    SDL_FRect bounds;
};
