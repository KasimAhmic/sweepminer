#pragma once

#include <chrono>

#include "cell.h"
#include "timer.h"
#include "include/framework.h"

enum Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};

class Game {
public:
    Game(HINSTANCE instanceHandle, HWND windowHandle);

    ~Game();

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
    std::vector<Cell*> cells;
    Timer* timer;
};
