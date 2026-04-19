#include "cell_grid.hpp"

#include <array>
#include <queue>
#include <random>
#include <unordered_set>

#include "events.hpp"
#include "util.hpp"

typedef std::pair<int32_t, int32_t> Offset;

constexpr Offset NORTH_WEST = {-1, -1};
constexpr Offset NORTH      = {+0, -1};
constexpr Offset NORTH_EAST = {+1, -1};
constexpr Offset EAST       = {+1, +0};
constexpr Offset SOUTH_EAST = {+1, +1};
constexpr Offset SOUTH      = {+0, +1};
constexpr Offset SOUTH_WEST = {-1, +1};
constexpr Offset WEST       = {-1, +0};

constexpr std::array EIGHT_DIR_CELL_OFFSETS = {
    NORTH_WEST, NORTH, NORTH_EAST,
    WEST,              EAST,
    SOUTH_WEST, SOUTH, SOUTH_EAST,
};

constexpr std::array FOUR_DIR_CELL_OFFSETS = {
    SOUTH,
    NORTH,
    WEST,
    EAST,
};

CellGrid::CellGrid(Context *context, const SDL_FRect &rect, const uint8_t rows, const uint8_t columns, const uint8_t mines)
    : Box(context, rect, BORDER_WIDTH, DARK_GREY, WHITE, GREY),
      rows(rows),
      columns(columns),
      mines(mines) {
    const uint16_t totalCells = this->columns * this->rows;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution distribution(0, totalCells - 1);

    uint16_t id = 0;
    std::unordered_set<uint16_t> mineCells;

    while (mineCells.size() < this->mines) {
        mineCells.insert(distribution(gen));
    }

    for (uint8_t row = 0; row < rows; row++) {
        std::vector<std::unique_ptr<Cell>> cellRow;

        cellRow.reserve(columns);

        for (uint8_t column = 0; column < columns; column++) {
            const SDL_FRect cellRect{
                .x = this->getBounds().x + BORDER_WIDTH + static_cast<float>(column) * Cell::SIZE * context->getScale(),
                .y = this->getBounds().y + BORDER_WIDTH + static_cast<float>(row) * Cell::SIZE * context->getScale(),
                .w = Cell::SIZE * context->getScale(),
                .h = Cell::SIZE * context->getScale()
            };

            cellRow.emplace_back(std::make_unique<Cell>(context, cellRect, row, column, mineCells.contains(id)));
            id++;
        }

        this->cells.emplace_back(std::move(cellRow));
    }

    for (uint8_t row = 0; row < this->rows; row++) {
        for (uint8_t column = 0; column < this->columns; column++) {
            uint8_t surroundingMineCount = 0;

            for (const auto [deltaColumn, deltaRow]: EIGHT_DIR_CELL_OFFSETS) {
                const int32_t newRow = row + deltaRow;
                const int32_t newColumn = column + deltaColumn;

                if (newRow < 0 ||
                    newRow >= this->rows ||
                    newColumn < 0 ||
                    newColumn >= this->columns) {
                    continue;
                }

                if (const Cell* cell = this->cells[newRow][newColumn].get(); cell != nullptr && cell->hasMine()) {
                    surroundingMineCount++;
                }
            }

            this->cells[row][column]->setSurroundingMines(surroundingMineCount);
        }
    }
}

CellGrid::~CellGrid() = default;

std::pair<float, float> CellGrid::getExpectedSize(const float scale, const uint8_t rows, const uint8_t columns) {
    return {
        scale * static_cast<float>(columns) * Cell::SIZE + BORDER_WIDTH * 2,
        scale * static_cast<float>(rows) * Cell::SIZE + BORDER_WIDTH * 2,
    };
}

void CellGrid::handleEvent(const SDL_Event &event) const {
    switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
            for (uint8_t row = 0; row < rows; row++) {
                for (uint8_t column = 0; column < columns; column++) {
                    this->cells[row][column]->handleMouseEvent(event.motion);
                }
            }

            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            for (uint8_t row = 0; row < rows; row++) {
                bool hit = false;

                for (uint8_t column = 0; column < columns; column++) {
                    hit = this->cells[row][column]->handleMouseEvent(event.button);

                    if (hit) break;
                }

                if (hit) break;
            }

            break;
        }

        default: {
            break;
        }
    }


    if (event.user.type == Events::REVEAL_CELL) {
        const auto [row, column] = Events::GetRevealedCell(event);
        this->revealConnectedCells(row, column);
    }
}

void CellGrid::revealConnectedCells(const uint8_t selectedCellRow, const uint8_t selectedCellColumn) const {
    std::queue<std::pair<int8_t, int8_t>> queue{};
    std::unordered_set<std::pair<int8_t, int8_t>, PairHash> visited{};

    queue.emplace(selectedCellRow, selectedCellColumn);

    while (!queue.empty()) {
        const auto [row, column] = queue.front();
        queue.pop();

        if (column < 0 ||
            column >= this->getColumns() ||
            row < 0 ||
            row >= this->getRows() ||
            visited.contains({row, column})) {
            continue;
        }

        visited.insert({row, column});

        const std::unique_ptr<Cell> &cell = this->cells[row][column];

        if (cell == nullptr || cell->hasMine()) {
            continue;
        }

        if (cell->getState() == Cell::State::FLAGGED) {
            // this->flags++;
        }

        if (!(row == selectedCellRow && column == selectedCellColumn)) {
            cell->setState(Cell::State::REVEALED);
        }

        if (cell->getSurroundingMines() > 0) {
            continue;
        }

        for (const auto [deltaRow, deltaColumn]: FOUR_DIR_CELL_OFFSETS) {
            queue.emplace(row + deltaRow, column + deltaColumn);
        }
    }
}

void CellGrid::render() {
    Box::render();

    SDL_SetRenderDrawColor(this->getContext().getRenderer(), SpreadColorInt(DARK_GREY));

    const float scale = this->getContext().getDisplayScale();
    const float padding = BORDER_WIDTH * scale;

    for (uint8_t i = 0; i < GRID_WIDTH * this->getContext().getDisplayScale(); i++) {
        for (uint8_t row = 1; row < rows; row++) {
            SDL_RenderLine(this->getContext().getRenderer(),
                           this->getRect().x + padding,
                           this->getRect().y + padding + static_cast<float>(row) * Cell::SIZE * scale + static_cast<float>(i),
                           this->getRect().x + this->getRect().w - padding - 1,
                           this->getRect().y + padding + static_cast<float>(row) * Cell::SIZE * scale + static_cast<float>(i));
        }

        for (uint8_t column = 1; column < columns; column++) {
            SDL_RenderLine(this->getContext().getRenderer(),
                           this->getRect().x + padding + static_cast<float>(column) * Cell::SIZE * scale + static_cast<float>(i),
                           this->getRect().y + padding,
                           this->getRect().x + padding + static_cast<float>(column) * Cell::SIZE * scale + static_cast<float>(i),
                           this->getRect().y + this->getRect().h - padding - 1);
        }
    }

    for (uint8_t row = 0; row < rows; row++) {
        for (uint8_t column = 0; column < columns; column++) {
            this->cells[row][column]->render();
        }
    }
}
