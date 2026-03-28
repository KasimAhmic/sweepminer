#pragma once

#include "button.hpp"
#include "color.hpp"

typedef void (*Callback)();

class Cell : public Button {
public:
    enum class State {
        UNKNOWN = -1,
        HIDDEN,
        FLAGGED,
        QUESTIONED,
        REVEALED,
    };

    explicit Cell(const AppContext &context,
                  const SDL_FRect &rect,
                  const uint16_t id,
                  const bool containsMine,
                  Callback mouseOverCallback = nullptr,
                  Callback mouseOutCallback = nullptr,
                  Callback mouseDownCallback = nullptr,
                  Callback mouseUpCallback = nullptr)
        : Button(context, rect),
          id(id),
          containsMine(containsMine),
          mouseOverCallback(mouseOverCallback),
          mouseOutCallback(mouseOutCallback),
          mouseDownCallback(mouseDownCallback),
          mouseUpCallback(mouseUpCallback) {
        this->setBackgroundColor(COLOR_BUTTON_DEFAULT)
                .setBorderHighlightColor(COLOR_BORDER_HIGHLIGHT)
                .setBorderShadowColor(COLOR_BORDER_SHADOW)
                .setBorderWidth(2);
    }

    void onMouseOver() override;
    void onMouseOut() override;
    void onMouseDown() override;
    void onMouseUp() override;

    [[nodiscard]] bool hasMine() const { return this->containsMine; }

    [[nodiscard]] uint8_t getSurroundingMines() const { return this->surroundingMines; }
    void setSurroundingMines(const uint8_t count) { this->surroundingMines = count; }

    [[nodiscard]] State getState() const { return this->state; }
    void setState(const State s) { this->state = s; }

    void render() override;

private:
    uint16_t id;
    bool containsMine;
    uint8_t surroundingMines = 0;
    State state = State::HIDDEN;
    Color backgroundColor = COLOR_BUTTON_DEFAULT;
    Callback mouseOverCallback;
    Callback mouseOutCallback;
    Callback mouseDownCallback;
    Callback mouseUpCallback;
};
