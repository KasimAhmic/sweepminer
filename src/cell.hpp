#pragma once

#include "SweepMiner/framework.h"

#include "resource_context.hpp"

class Game; // Forward declaration

constexpr int32_t SCALE = 4;
constexpr int32_t CELL_SIZE = 16 * SCALE;
constexpr int32_t CELL_PADDING = 0;
constexpr int32_t IMAGE_PADDING = 3 * SCALE;
constexpr int32_t BORDER_WIDTH = 2 * SCALE;

enum class State {
    HIDDEN,
    FLAGGED,
    QUESTIONED,
    REVEALED,
};

class Cell {
public:
    Cell(
        Game &game,
        const std::shared_ptr<ResourceContext> &resourceContext,
        HINSTANCE instanceHandle,
        HWND windowHandle,
        int32_t id,
        int32_t xPosition,
        int32_t yPosition,
        int32_t column,
        int32_t row,
        bool hasMine);

    ~Cell();

    static LRESULT CALLBACK BoxProc(
        HWND windowHandle,
        UINT message,
        WPARAM wordParam,
        LPARAM longParam,
        UINT_PTR idSubclass,
        DWORD_PTR cellPointer);

    void mark();
    void reveal();
    void revealCell();
    void setSurroundingMineCount(int32_t count);

    [[nodiscard]] HWND getHandle() const { return this->handle; }
    [[nodiscard]] int32_t getSurroundingMineCount() const { return this->surroundingMineCount; }
    [[nodiscard]] bool hasMine() const { return this->containsMine; }
    [[nodiscard]] bool isRevealed() const { return this->state == State::REVEALED; }
    [[nodiscard]] bool isMarked() const { return this->state == State::FLAGGED || this->state == State::QUESTIONED; }

private:
    Game& game;
    int32_t id;
    HWND handle;
    State state;
    int32_t surroundingMineCount;
    bool containsMine;
    int32_t row;
    int32_t column;
    std::shared_ptr<ResourceContext> resources;

    static void DrawBorder(HDC hdc, LPRECT rect);
};
