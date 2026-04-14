#pragma once
#include "box.hpp"

class Counter : public Box {
public:
    static constexpr float BORDER_WIDTH = 2.0f;

    explicit Counter(Context* context, const SDL_FRect& rect);
    ~Counter() override;

    void render() override;
};
