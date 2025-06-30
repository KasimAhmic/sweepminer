#include "mouse.hpp"

std::pair<int32_t, int32_t> Mouse::position = { 0, 0 };
MouseState Mouse::state = MouseState::UP;
MouseEvent Mouse::event;
MouseButton Mouse::button;

std::optional<std::pair<int32_t, int32_t>> Mouse::getCellOffsets() {
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

void Mouse::setButton(const uint8_t newButton) {
    if (newButton == SDL_BUTTON_LEFT) {
        Mouse::button = MouseButton::LEFT;
    } else if (newButton == SDL_BUTTON_RIGHT) {
        Mouse::button = MouseButton::RIGHT;
    }
}
