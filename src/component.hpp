#pragma once

#include "context.hpp"

class Component {
public:
    explicit Component(const AppContext& context, const SDL_FRect rect): context(context), rect(rect) {}
    virtual ~Component() = default;

    virtual void render() = 0;

protected:
    AppContext context;
    SDL_FRect rect;
};
