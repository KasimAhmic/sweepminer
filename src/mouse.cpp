#include "mouse.hpp"

std::pair<int32_t, int32_t> Mouse::position = { 0, 0 };
std::pair<int32_t, int32_t> Mouse::eventPosition = { 0, 0 };
MouseState Mouse::state;
MouseEvent Mouse::event;
MouseButton Mouse::button;

void Mouse::setButton(const uint8_t newButton) {
    if (newButton == SDL_BUTTON_LEFT) {
        Mouse::button = MouseButton::LEFT;
    } else if (newButton == SDL_BUTTON_RIGHT) {
        Mouse::button = MouseButton::RIGHT;
    }
}

bool Mouse::withinRegion(const SDL_FRect *region) {
    const auto [mouseX, mouseY] = Mouse::getPosition();

    return (static_cast<float>(mouseX) >= region->x && static_cast<float>(mouseX) <= region->x + region->w &&
            static_cast<float>(mouseY) >= region->y && static_cast<float>(mouseY) <= region->y + region->h);
}

bool Mouse::eventStartedWithinRegion(const SDL_FRect *region) {
    const auto [mouseX, mouseY] = Mouse::getEventPosition();

    return (static_cast<float>(mouseX) >= region->x && static_cast<float>(mouseX) <= region->x + region->w &&
            static_cast<float>(mouseY) >= region->y && static_cast<float>(mouseY) <= region->y + region->h);
}

bool Mouse::isLeftClicking() {
    return Mouse::getButton() == MouseButton::LEFT && Mouse::getState() == MouseState::DOWN;
}

bool Mouse::isRightClicking() {
    return Mouse::getButton() == MouseButton::RIGHT && Mouse::getState() == MouseState::DOWN;
}
