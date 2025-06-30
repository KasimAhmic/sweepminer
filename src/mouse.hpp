#pragma once

#include <optional>
#include <utility>

#include "cell.hpp"
#include "constants.hpp"
#include "scaler.hpp"

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

    [[nodiscard]] static MouseState getState() { return Mouse::state; }
    static void setState(const MouseState newState) { Mouse::state = newState; }

    [[nodiscard]] static MouseEvent getEvent() { return Mouse::event; }
    static void setEvent(const MouseEvent newEvent) { Mouse::event = newEvent; }

    [[nodiscard]] static MouseButton getButton() { return Mouse::button; }
    static void setButton(const MouseButton newButton) { Mouse::button = newButton; }
    static void setButton(uint8_t newButton);

    static std::optional<std::pair<int32_t, int32_t>> getCellOffsets();

private:
    /**
     * The column and row offsets of the cells that the mouse is over.
     */
    static std::pair<int32_t, int32_t> position;

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
