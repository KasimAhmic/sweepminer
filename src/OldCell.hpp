#pragma once

#include <optional>

#include <SDL3/SDL.h>

#include "context.hpp"
#include "resource_context.hpp"

class Game; // Forward declaration

class OldCell {
public:
    enum class OldState {
        HIDDEN,
        FLAGGED,
        QUESTIONED,
        REVEALED,
    };

    OldCell(const AppContext &context,
        uint16_t id,
        float xPosition,
        float yPosition,
        uint8_t column,
        uint8_t row,
        bool containsMine,
        const std::shared_ptr<ResourceContext> &resourceContext);

    ~OldCell() = default;

    [[nodiscard]] uint16_t getId() const { return this->id; }
    [[nodiscard]] float getXPosition() const { return this->xPosition; }
    [[nodiscard]] float getYPosition() const { return this->yPosition; }
    [[nodiscard]] uint8_t getColumn() const { return this->column; }
    [[nodiscard]] uint8_t getRow() const { return this->row; }
    [[nodiscard]] bool hasMine() const { return this->containsMine; }

    [[nodiscard]] uint8_t getSurroundingMines() const { return this->surroundingMines; }
    void setSurroundingMines(const uint8_t surroundingMines) { this->surroundingMines = surroundingMines; }

    [[nodiscard]] OldState getState() const { return this->state; }
    void setState(const OldCell::OldState state) { this->state = state; }

    void draw(SDL_Renderer* renderer) const;
    std::optional<std::pair<uint16_t, uint16_t>> reveal();

private:
    AppContext context;
    uint16_t id;
    float xPosition;
    float yPosition;
    uint8_t column;
    uint8_t row;
    OldState state;
    uint8_t surroundingMines;
    bool containsMine;
    std::shared_ptr<ResourceContext> resourceContext;

    void drawGrid(SDL_Renderer *renderer) const;
};
