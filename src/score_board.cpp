#include "score_board.hpp"

ScoreBoard::ScoreBoard(Context* context, const SDL_FRect& rect)
    : Box(context, rect, BORDER_WIDTH, DARK_GREY, WHITE, GREY) {
    this->flags = std::make_unique<Counter>(context, SDL_FRect{
        .x = rect.x + PADDING,
        .y = rect.y + PADDING,
        .w = WIDTH,
        .h = rect.h - PADDING * 2
    });
    this->timer = std::make_unique<Counter>(context, SDL_FRect{
        .x = rect.x + rect.w - WIDTH - PADDING,
        .y = rect.y + PADDING,
        .w = WIDTH,
        .h = rect.h - PADDING * 2
    });
}

ScoreBoard::~ScoreBoard() = default;

void ScoreBoard::render() {
    Box::render();

    this->flags->render();
    this->timer->render();
}
