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

    SDL_FRect newGame(uint8_t columns, uint8_t rows, uint16_t mines);
    SDL_FRect newGame(Difficulty difficulty);

    void loadResources(SDL_Renderer* renderer) const;
    void draw(SDL_Renderer *renderer, int32_t windowWidth, int32_t windowHeight) const;
    void start() const;
    void end() const;
    void tick();
    void revealConnectedCells(uint16_t x, uint16_t y);
    void handleMouseEvent();

    [[nodiscard]] uint8_t getColumns() const { return this->columns; }
    [[nodiscard]] uint8_t getRows() const { return this->rows; }
    [[nodiscard]] uint16_t getMines() const { return this->mines; }
    [[nodiscard]] uint16_t getFlags() const { return this->flags; }
    [[nodiscard]] uint16_t getClock() const { return this->clock; }
    [[nodiscard]] bool isRunning() const { return this->running; }

private:
    uint8_t columns;
    uint8_t rows;
    uint16_t mines;
    uint16_t flags;
    bool running;
    uint16_t clock;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
    std::unique_ptr<Timer> timer;
    std::shared_ptr<ResourceContext> resourceContext;

    static SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    void drawScoreboard(SDL_Renderer *renderer, uint32_t windowWidth) const;
    [[nodiscard]] static std::array<uint8_t, 3> getDisplayDigits(uint16_t value);
};
