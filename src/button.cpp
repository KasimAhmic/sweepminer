#include "button.hpp"

Button::Button(const SDL_FRect &rect)
    : rect(rect),
      isHovered(false),
      isClicked(false) {}

Button::~Button() = default;

bool Button::handleMouseEvent(const SDL_MouseMotionEvent& event) {
    const bool wasHovered = this->isHovered;
    this->isHovered = this->isWithinBounds(event.x, event.y);

    if (!wasHovered && this->isHovered) {
        this->onMouseOver(event);
        return true;
    }

    if (wasHovered && !this->isHovered) {
        this->onMouseOut(event);
        return true;
    }

    if (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
        if (this->isHovered) {
            this->isHovered = false;
            this->onMouseOut(event);
            return true;
        }
    }

    return false;
}

bool Button::handleMouseEvent(const SDL_MouseButtonEvent& event) {
    if (!this->isHovered) {
        return false;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        this->isClicked = true;
        this->onMouseDown(event);
        return true;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        this->isClicked = false;
        this->onMouseUp(event);
        return true;
    }

    return false;
}
