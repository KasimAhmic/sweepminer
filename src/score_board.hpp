#pragma once
#include <memory>

#include "box.hpp"
#include "context.hpp"
#include "counter.hpp"

class ScoreBoard : public Box {
public:
    static constexpr float WIDTH = 50.0f;
    static constexpr float HEIGHT = 60.0f;
    static constexpr float BORDER_WIDTH = 3.0f;
    static constexpr float PADDING = 10.0f;

    explicit ScoreBoard(Context* context, const SDL_FRect& rect);
    ~ScoreBoard() override;

    void render() override;

private:
    std::unique_ptr<Counter> flags;
    std::unique_ptr<Counter> timer;
};
