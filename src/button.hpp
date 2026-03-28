#pragma once

#include "box.hpp"
#include "component.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_rect.h"

class Button : public Box {
public:
    explicit Button(const AppContext& context, const SDL_FRect& rect)
        : Box(context, rect),
            isHovered(false),
            isClicked(false) {}
    ~Button() override = default;

    virtual void onMouseOver() = 0;
    virtual void onMouseOut() = 0;
    virtual void onMouseDown() = 0;
    virtual void onMouseUp() = 0;

    bool handleMouseEvent(const SDL_MouseMotionEvent& event);
    bool handleMouseEvent(const SDL_MouseButtonEvent& event);

private:
    bool isHovered;
    bool isClicked;

    // TODO: Decide if this needs to be moved up to Component, or out to a utility function
    [[nodiscard]] bool isWithinBounds(const float x, const float y) const {
        return x > this->rect.x &&
               x < this->rect.x + this->rect.w &&
               y > this->rect.y &&
               y < this->rect.y + this->rect.h;
    }
};
