#pragma once

#include <memory>
#include <vector>

#include "SDL3/SDL.h"

#include "cell.hpp"
#include "resource_context.hpp"
#include "timer.hpp"

enum class Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};

class Game {
public:
    Game();
    ~Game() = default;

    SDL_Rect newGame(uint8_t columns, uint8_t rows, uint16_t mines);
    SDL_Rect newGame(Difficulty difficulty);

    void loadResources(SDL_Renderer* renderer) const;
    void draw(SDL_Renderer *renderer, int32_t windowWidth, int32_t windowHeight) const;
    void start() const;
    void end() const;
    void tick();
    void revealConnectedCells(uint16_t x, uint16_t y) const;
    void handleMouseEvent() const;

    [[nodiscard]] uint8_t getColumns() const { return columns; }
    [[nodiscard]] uint8_t getRows() const { return rows; }
    [[nodiscard]] uint16_t getMines() const { return mines; }
    [[nodiscard]] uint16_t getFlags() const { return flags; }
    [[nodiscard]] bool isRunning() const { return running; }
    [[nodiscard]] uint64_t getClock() const { return clock; }

private:
    uint8_t columns;
    uint8_t rows;
    uint16_t mines;
    uint16_t flags;
    bool running;
    uint64_t clock;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
    std::unique_ptr<Timer> timer;
    std::shared_ptr<ResourceContext> resourceContext;

    void loadCellCountTexture(SDL_Renderer* renderer) const;
};
