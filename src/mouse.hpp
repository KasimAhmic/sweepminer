#pragma once

#include <cstdint>
#include <utility>

enum class MouseState {
    UP,
    DOWN,
};

class Mouse {
public:
    [[nodiscard]] static std::pair<int32_t, int32_t> getPosition() { return Mouse::position; }
    static void setPosition(const int32_t x, const int32_t y) { Mouse::position = { x, y }; }

    [[nodiscard]] static MouseState getState() { return Mouse::state; }
    static void setState(const MouseState newState) { Mouse::state = newState; }

private:
    /**
     * The column and row offsets of the cells that the mouse is over.
     */
    static std::pair<int32_t, int32_t> position;

    /**
     * The current state of the mouse.
     */
    static MouseState state;
};
