#include "mouse.hpp"

std::pair<int32_t, int32_t> Mouse::position = { 0, 0 };
MouseState Mouse::state = MouseState::UP;
MouseEvent Mouse::event;
MouseButton Mouse::button;
