#pragma once

#include "box.hpp"
#include "button.hpp"

class Cell : public Box, public Button {
public:
    static constexpr float SIZE = 16.0f;
    static constexpr float BORDER_WIDTH = 2.0f;

    enum class State {
        HIDDEN,
        FLAGGED,
        QUESTIONED,
        REVEALED,
        EXPLODED,
    };

    explicit Cell(Context* context, const SDL_FRect& rect, uint8_t row, uint8_t column, bool containsMine);
    ~Cell() override;

    [[nodiscard]] State getState() const { return this->state; }
    void setState(const State newState) { this->state = newState; }

    [[nodiscard]] uint8_t getSurroundingMines() const { return this->surroundingMines; }
    void setSurroundingMines(const uint8_t mines) { this->surroundingMines = mines; }

    [[nodiscard]] bool hasMine() const { return this->containsMine; }

    void render() override;

protected:
    void onMouseOver(const SDL_MouseMotionEvent& event) override;
    void onMouseOut(const SDL_MouseMotionEvent& event) override;
    void onMouseDown(const SDL_MouseButtonEvent& event) override;
    void onMouseUp(const SDL_MouseButtonEvent& event) override;

private:
    State state;
    uint8_t row;
    uint8_t column;
    uint8_t surroundingMines;
    bool containsMine;
};
