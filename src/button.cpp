#include "button.hpp"

bool Button::handleMouseEvent(const SDL_MouseMotionEvent& event) {
    const bool wasHovered = this->isHovered;
    this->isHovered = this->isWithinBounds(event.x, event.y);

    if (!wasHovered && this->isHovered) {
        this->onMouseOver();
        return true;
    }

    if (wasHovered && !this->isHovered) {
        this->onMouseOut();
        return true;
    }

    if (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
        if (this->isHovered) {
            this->isHovered = false;
            this->onMouseOut();
            return true;
        }
    }

    return false;
}

bool Button::handleMouseEvent(const SDL_MouseButtonEvent& event) {
    if (!this->isHovered) {
        return false;
    }

    SDL_Log("Mouse Button: %d, State: %s", event.button, event.down ? "DOWN" : "UP");

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        this->isClicked = true;
        this->onMouseDown();
        return true;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        this->isClicked = false;
        this->onMouseUp();
        return true;
    }

    return false;
}
