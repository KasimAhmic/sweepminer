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

class Mouse {
public:
    [[nodiscard]] static std::pair<int32_t, int32_t> getPosition() { return Mouse::position; }
    static void setPosition(const int32_t x, const int32_t y) { Mouse::position = { x, y }; }

    [[nodiscard]] static MouseState getState() { return Mouse::state; }
    static void setState(const MouseState newState) { Mouse::state = newState; }

    [[nodiscard]] static MouseEvent getEvent() { return Mouse::event; }
    static void setEvent(const MouseEvent newEvent) { Mouse::event = newEvent; }

    static std::optional<std::pair<int32_t, int32_t>> getCellOffsets() {
        const auto [mouseX, mouseY] = Mouse::getPosition();

        const int32_t gridX = mouseX / Scaler::getUserScale() - CELL_GRID_OFFSET_X - THICK_BORDER_WIDTH * 2;
        const int32_t gridY = mouseY / Scaler::getUserScale() - CELL_GRID_OFFSET_Y - THICK_BORDER_WIDTH * 2;

        if (gridX < 0 || gridY < 0) {
            return std::nullopt;
        }

        const int32_t column = gridX / CELL_SIZE;
        const int32_t row = gridY / CELL_SIZE;

        return std::make_pair(column, row);
    }

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
};
