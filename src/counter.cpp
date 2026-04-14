#include "counter.hpp"

Counter::Counter(Context *context, const SDL_FRect &rect)
    : Box(context, rect, BORDER_WIDTH, DARK_GREY, WHITE, GREY) {}

Counter::~Counter() = default;

void Counter::render() {
    Box::render();
}
