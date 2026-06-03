#pragma once

#include <memory>
#include <utility>

#include "box.hpp"
#include "cell.hpp"

class CellGrid : public Box {
public:
    static constexpr float BORDER_WIDTH = 3.0f;
    static constexpr uint8_t GRID_WIDTH = 1;

    explicit CellGrid(Context* context, const SDL_FRect& rect, uint8_t rows, uint8_t columns, uint8_t mines);
    ~CellGrid() override;

    void render() override;

    [[nodiscard]] static std::pair<float, float> getExpectedSize(float scale, uint8_t rows, uint8_t columns);

    [[nodiscard]] uint8_t getRows() const { return this->rows; }
    [[nodiscard]] uint8_t getColumns() const { return this->columns; }
    [[nodiscard]] uint8_t getMines() const { return this->mines; }

    void handleEvent(const SDL_Event &event) const;
    void revealConnectedCells(uint8_t selectedCellRow, uint8_t selectedCellColumn) const;
    void checkForVictory() const;

private:
    uint8_t rows;
    uint8_t columns;
    uint8_t mines;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
};
