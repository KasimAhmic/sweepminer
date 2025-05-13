#pragma once

#include <chrono>

#include "cell.hpp"
#include "timer.hpp"
#include "include/framework.h"

enum Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};

class Game {
public:
    Game(HINSTANCE instanceHandle, HWND windowHandle);
    ~Game() = default;

    RECT start(int width, int height, int mines);
    RECT start(Difficulty difficulty);
    void end() const;
    void tick();

private:
    HINSTANCE instanceHandle;
    HWND windowHandle;
    int width;
    int height;
    int mines;
    int clock;
    int flags;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
    std::unique_ptr<Timer> timer;
    std::shared_ptr<ResourceContext> resourceContext;

    ResourceContext LoadResources() const;
};
