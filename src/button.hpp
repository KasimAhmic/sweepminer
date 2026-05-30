#pragma once

#include <SDL3/SDL.h>

class Button {
public:
    explicit Button(const SDL_FRect& rect);
    virtual ~Button();

    [[nodiscard]] const SDL_FRect& getRect() const { return this->rect; }

    bool handleMouseEvent(const SDL_MouseMotionEvent& event);
    bool handleMouseEvent(const SDL_MouseButtonEvent& event);

protected:
    virtual void onMouseOver(const SDL_MouseMotionEvent& event) = 0;
    virtual void onMouseOut(const SDL_MouseMotionEvent& event) = 0;
    virtual void onMouseDown(const SDL_MouseButtonEvent& event) = 0;
    virtual void onMouseUp(const SDL_MouseButtonEvent& event) = 0;

private:
    SDL_FRect rect;
    bool isHovered;
    bool isClicked;

    [[nodiscard]] bool isWithinBounds(const float x, const float y) const {
        return x > this->getRect().x &&
               x < this->getRect().x + this->getRect().w &&
               y > this->getRect().y &&
               y < this->getRect().y + this->getRect().h;
    }
};
