#include "score_board.hpp"

#include <cmath>

#include "events.hpp"

ScoreBoard::ScoreBoard(Context* context, const SDL_FRect& rect)
    : Box(context, rect, BORDER_WIDTH, DARK_GREY, WHITE, GREY) {
    this->flags = std::make_unique<Counter>(context, SDL_FRect{
        .x = rect.x + PADDING,
        .y = rect.y + PADDING,
        .w = WIDTH,
        .h = rect.h - PADDING * 2
    });
    this->clock = std::make_unique<Counter>(context, SDL_FRect{
        .x = rect.x + rect.w - WIDTH - PADDING,
        .y = rect.y + PADDING,
        .w = WIDTH,
        .h = rect.h - PADDING * 2
    });
    this->newGameButton = std::make_unique<NewGameButton>(context, SDL_FRect{
        .x = round(rect.x + rect.w / 2 - NewGameButton::SIZE / 2),
        .y = round(rect.y + rect.h / 2 - NewGameButton::SIZE / 2),
        .w = NewGameButton::SIZE,
        .h = NewGameButton::SIZE
    });
}

ScoreBoard::~ScoreBoard() = default;

void ScoreBoard::render() {
    Box::render();

    this->flags->render();
    this->clock->render();
    this->newGameButton->render();
}

void ScoreBoard::tick() const {
    this->clock->increment();
}

void ScoreBoard::handleEvent(const SDL_Event &event) const {
    switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (!this->newGameButton->handleMouseEvent(event.button)) {
                if (this->newGameButton->getState() != NewGameButton::State::DEFEAT) {
                    this->newGameButton->setState(NewGameButton::State::WORRIED);
                }
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (!this->newGameButton->handleMouseEvent(event.button)) {
                if (this->newGameButton->getState() != NewGameButton::State::DEFEAT) {
                    this->newGameButton->setState(NewGameButton::State::DEFAULT);
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
            this->newGameButton->handleMouseEvent(event.motion);
            break;
        }

        default:
            break;
    }

    if (event.type == Events::MARK_CHANGE) {
        if (event.user.code == 1) {
            this->flags->increment();
        } else if (event.user.code == -1) {
            this->flags->decrement();
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Received MARK_CHANGE event with invalid code: %d", event.user.code);
        }
    } else if (event.type == Events::LOSE_GAME) {
        this->newGameButton->setState(NewGameButton::State::DEFEAT);
    }
}
