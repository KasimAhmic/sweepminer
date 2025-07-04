#pragma once

#include <optional>

#include <SDL3/SDL.h>

#include "color.hpp"
#include "resource_context.hpp"

class Game; // Forward declaration

constexpr Color BACKGROUND_COLOR(192, 192, 192, 255);
constexpr Color HOVERED_COLOR(150, 150, 150, 255);
constexpr Color BORDER_HIGHLIGHT_COLOR(255, 255, 255, 255);
constexpr Color BORDER_SHADOW_COLOR(128, 128, 128, 255);

enum class State {
    HIDDEN,
    FLAGGED,
    QUESTIONED,
    REVEALED,
};

class Cell {
public:
    Cell(uint16_t id,
        uint16_t xPosition,
        uint16_t yPosition,
        uint8_t column,
        uint8_t row,
        bool containsMine,
        const std::shared_ptr<ResourceContext> &resourceContext);

    ~Cell() = default;

    void draw(SDL_Renderer *renderer) const;
    bool mark();
    std::optional<std::pair<uint16_t, uint16_t>> reveal();
    void revealCell();

    void setSurroundingMines(const uint8_t surroundingMines) { this->surroundingMines = surroundingMines; }

    [[nodiscard]] uint16_t getId() const { return this->id; }
    [[nodiscard]] uint16_t getXPosition() const { return this->xPosition; }
    [[nodiscard]] uint16_t getYPosition() const { return this->yPosition; }
    [[nodiscard]] uint8_t getColumn() const { return this->column; }
    [[nodiscard]] uint8_t getRow() const { return this->row; }
    [[nodiscard]] State getState() const { return this->state; }
    [[nodiscard]] uint8_t getSurroundingMines() const { return this->surroundingMines; }
    [[nodiscard]] bool hasMine() const { return this->containsMine; }

private:
    uint16_t id;
    uint16_t xPosition;
    uint16_t yPosition;
    uint8_t column;
    uint8_t row;
    State state;
    uint8_t surroundingMines;
    bool containsMine;
    std::shared_ptr<ResourceContext> resourceContext;

    void drawGrid(SDL_Renderer *renderer) const;
};
