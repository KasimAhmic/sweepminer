#pragma once
#include "box.hpp"
#include "button.hpp"

class NewGameButton : public Box, public Button {
public:
    static constexpr float SIZE = 25.0f;
    static constexpr float BORDER_WIDTH = 2.0f;
    static constexpr float PADDING = 4.0f;

    enum class State : uint8_t {
        DEFAULT,
        PRESSED,
        WORRIED,
        DEFEAT,
        VICTORY,
    };

    explicit NewGameButton(Context* context, const SDL_FRect& rect);
    ~NewGameButton() override;

    void render() override;

    [[nodiscard]] State getState() const { return this->state; }
    void setState(const State newState) { this->state = newState; }

protected:
    void onMouseOver(const SDL_MouseMotionEvent& event) override;
    void onMouseOut(const SDL_MouseMotionEvent& event) override;
    void onMouseDown(const SDL_MouseButtonEvent& event) override;
    void onMouseUp(const SDL_MouseButtonEvent& event) override;

private:
    State state = State::DEFAULT;
};
