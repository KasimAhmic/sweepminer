#pragma once

#include <utility>
#include <imgui.h>

#include "cell.hpp"

enum class MouseState {
    UP,
    DOWN,
};

enum class MouseEvent {
    MOVE,
    BUTTON_DOWN,
    BUTTON_UP,
    ENTER,
    LEAVE,
};

enum class MouseButton {
    LEFT,
    RIGHT,
};

class Mouse {
public:
    [[nodiscard]] static std::pair<int32_t, int32_t> getPosition() { return Mouse::position; }
    static void setPosition(const int32_t x, const int32_t y) { Mouse::position = { x, y }; }

    [[nodiscard]] static std::pair<int32_t, int32_t> getEventPosition() { return Mouse::eventPosition; }
    static void setEventPosition(const int32_t x, const int32_t y) { Mouse::eventPosition = { x, y }; }

    [[nodiscard]] static MouseState getState() { return Mouse::state; }
    static void setState(const MouseState state) { Mouse::state = state; }

    [[nodiscard]] static MouseEvent getEvent() { return Mouse::event; }
    static void setEvent(const MouseEvent event) { Mouse::event = event; }

    [[nodiscard]] static MouseButton getButton() { return Mouse::button; }
    static void setButton(const MouseButton button) { Mouse::button = button; }
    static void setButton(uint8_t button);

    static bool isLeftClicking();
    static bool isRightClicking();

    static bool withinRegion(const SDL_FRect* region);
    static bool eventStartedWithinRegion(const SDL_FRect* region);
    static bool isHoveringImGuiWindow();

private:
    /**
     * The X and Y position of the mouse cursor.
     */
    static std::pair<int32_t, int32_t> position;

    /**
     * The position of the last mouse event.
     */
    static std::pair<int32_t, int32_t> eventPosition;

    /**
     * The current state of the mouse.
     */
    static MouseState state;

    /**
     * The last mouse event that occurred.
     */
    static MouseEvent event;

    /**
     * The mouse button that was pressed or released.
     */
    static MouseButton button;
};
