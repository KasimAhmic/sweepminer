#pragma once

#include <memory>

#include "box.hpp"
#include "context.hpp"
#include "counter.hpp"
#include "new_game_button.hpp"

class ScoreBoard : public Box {
public:
    static constexpr float BORDER_WIDTH = 2.0f;
    static constexpr float PADDING = 6.0f;
    static constexpr float WIDTH = 41.0f;
    static constexpr float HEIGHT = 37.0f;

    explicit ScoreBoard(Context* context, const SDL_FRect& rect);
    ~ScoreBoard() override;

    void render() override;
    void tick() const;
    void handleEvent(const SDL_Event &event) const;

private:
    std::unique_ptr<Counter> flags;
    std::unique_ptr<Counter> clock;
    std::unique_ptr<NewGameButton> newGameButton;
};
