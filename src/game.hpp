#pragma once

#include <chrono>

#include "cell.hpp"
#include "timer.hpp"

enum Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};

class Game {
public:
    Game(HINSTANCE instanceHandle, HWND windowHandle);
    ~Game() = default;

    RECT start(int32_t width, int32_t height, int32_t mines);
    RECT start(Difficulty difficulty);
    void end() const;
    void tick();
    void revealConnectedEmptyCells(int32_t selectedCellX, int32_t selectedCellY) const;

    void showMines() const;
    void showCounts() const;
    void revealAll() const;

private:
    HINSTANCE instanceHandle;
    HWND windowHandle;
    int32_t columnCount;
    int32_t rowCount;
    int32_t mineCount;
    int32_t flagCount;
    int32_t clock;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
    std::unique_ptr<Timer> timer;
    std::shared_ptr<ResourceContext> resourceContext;

    [[nodiscard]] ResourceContext LoadResources() const;
};
