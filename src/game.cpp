#include "game.hpp"

#include <queue>
#include <set>
#include <unordered_set>
#include <random>

#include "SweepMiner/resource.hpp"

#include "cell.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "pair_hash.hpp"
#include "util.hpp"

static auto *logger = new logging::Logger("Game");

typedef std::pair<int32_t, int32_t> Offset;

constexpr Offset NORTH_WEST = {-1, -1};
constexpr Offset NORTH =      {+0, -1};
constexpr Offset NORTH_EAST = {+1, -1};
constexpr Offset EAST =       {+1, +0};
constexpr Offset SOUTH_EAST = {+1, +1};
constexpr Offset SOUTH =      {+0, +1};
constexpr Offset SOUTH_WEST = {-1, +1};
constexpr Offset WEST =       {-1, +0};
constexpr Offset TEST =       {-1, +0};

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

Game::Game(HINSTANCE instanceHandle, HWND windowHandle) {
    this->instanceHandle = instanceHandle;
    this->windowHandle = windowHandle;
    this->columnCount = 0;
    this->rowCount = 0;
    this->mineCount = 0;
    this->clock = 0;
    this->flagCount = 0;
    this->timer = std::make_unique<Timer>([this] { this->tick(); }, 1000);
    this->resourceContext = std::make_shared<ResourceContext>(LoadResources());
}

ResourceContext Game::LoadResources() const {
    auto *ctx = new ResourceContext();

    ctx->Add(Image::MINE, LoadImageFromResource(instanceHandle, IDR_MINE));
    ctx->Add(Image::FLAG, LoadImageFromResource(instanceHandle, IDR_FLAG));
    ctx->Add(Image::QUESTION, LoadImageFromResource(instanceHandle, IDR_QUESTION));

    ctx->Add(Brush::HIDDEN_BACKGROUND, MakeSolidBrush(RGB(192, 192, 192)));
    ctx->Add(Brush::BORDER_HIGHLIGHT, MakeSolidBrush(RGB(255, 255, 255)));
    ctx->Add(Brush::BORDER_SHADOW, MakeSolidBrush(RGB(128, 128, 128)));
    ctx->Add(Brush::EXPLODED_BACKGROUND, MakeSolidBrush(RGB(255, 0, 0)));

    ctx->Add(Font::NUMBER, MakeFont(
                 -CELL_SIZE / 2,
                 0,
                 0,
                 0,
                 FW_BOLD,
                 false,
                 false,
                 false,
                 DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS,
                 CLIP_DEFAULT_PRECIS,
                 CLEARTYPE_QUALITY,
                 VARIABLE_PITCH,
                 L"Segoe UI"));

    return *ctx;
}

RECT Game::start(const Difficulty difficulty) {
    switch (difficulty) {
        case BEGINNER: return start(9, 9, 10);
        case INTERMEDIATE: return start(16, 16, 40);
        case EXPERT: return start(30, 16, 99);
    }

    throw std::invalid_argument("Invalid difficulty");
}

RECT Game::start(const int32_t width, const int32_t height, const int32_t mines) {
    this->columnCount = width;
    this->rowCount = height;
    this->mineCount = mines;

    const int32_t totalCells = this->columnCount * this->rowCount;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution distribution(0, totalCells - 1);

    int32_t id = IDC_FIRST_BOX;
    std::set<int32_t> mineCells;

    // TODO: Remove test values
    mineCells.insert(4);
    mineCells.insert(14);
    mineCells.insert(24);
    mineCells.insert(34);
    mineCells.insert(88);
    mineCells.insert(50);
    mineCells.insert(61);
    mineCells.insert(72);
    mineCells.insert(83);
    mineCells.insert(94);

    while (mineCells.size() < this->mineCount) {
        mineCells.insert(distribution(gen));
    }

    for (int32_t row = 0; row < this->rowCount; row++) {
        std::vector<std::unique_ptr<Cell>> cellRow;

        for (int32_t column = 0; column < this->columnCount; column++) {
            cellRow.push_back(std::make_unique<Cell>(
                *this,
                this->resourceContext,
                this->instanceHandle,
                this->windowHandle,
                id,
                column * CELL_SIZE,
                row * CELL_SIZE,
                column,
                row,
                mineCells.contains(id - IDC_FIRST_BOX)));

            logger->debug(id, " ", id - IDC_FIRST_BOX);

            id++;
        }

        this->cells.push_back(std::move(cellRow));
    }

    for (int32_t row = 0; row < this->rowCount; row++) {
        for (int32_t column = 0; column < this->columnCount; column++) {
            int32_t surroundingMineCount = 0;

            for (const auto [deltaColumn, deltaRow]: EIGHT_DIR_CELL_OFFSETS) {
                const int32_t newRow = row + deltaRow;
                const int32_t newColumn = column + deltaColumn;

                if (newRow < 0 ||
                    newRow >= this->rowCount ||
                    newColumn < 0 ||
                    newColumn >= this->columnCount) {
                    continue;
                }

                if (const Cell *cell = this->cells[newRow][newColumn].get(); cell != nullptr && cell->hasMine()) {
                    surroundingMineCount++;
                }
            }

            this->cells[row][column]->setSurroundingMineCount(surroundingMineCount);
        }
    }

    return {0, 0, this->columnCount * CELL_SIZE, this->rowCount * CELL_SIZE};
}

void Game::end() const {
    this->timer->stop();
}

void Game::tick() {
    this->clock++;
}

void Game::revealConnectedEmptyCells(int32_t selectedCellX, int32_t selectedCellY) const {
    std::queue<std::pair<int32_t, int32_t>> queue;
    std::unordered_set<std::pair<int32_t, int32_t>, PairHash> visited;

    queue.emplace(selectedCellX, selectedCellY);

    while (!queue.empty()) {
        const auto [column, row] = queue.front();
        queue.pop();

        if (column < 0 ||
            column >= this->columnCount ||
            row < 0 ||
            row >= this->rowCount ||
            visited.contains({column, row})) {
            continue;
        }

        visited.insert({column, row});

        const std::unique_ptr<Cell> &cell = this->cells[row][column];

        if (cell == nullptr ||
            cell->hasMine() ||
            cell->isRevealed()) {
            continue;
        }

        cell->revealCell();

        if (cell->getSurroundingMineCount() > 0) {
            continue;
        }

        for (const auto [deltaColumn, deltaRow]: FOUR_DIR_CELL_OFFSETS) {
            queue.emplace(column + deltaColumn, row + deltaRow);
        }
    }
}

void Game::showMines() const {
    for (int32_t row = 0; row < this->rowCount; row++) {
        for (int32_t column = 0; column < this->columnCount; column++) {
            if (Cell *cell = this->cells[row][column].get(); cell != nullptr && cell->hasMine()) {
                cell->revealCell();
            }
        }
    }
}

void Game::showCounts() const {
    for (int32_t row = 0; row < this->rowCount; row++) {
        for (int32_t column = 0; column < this->columnCount; column++) {
            if (Cell *cell = this->cells[row][column].get(); cell != nullptr && cell->getSurroundingMineCount() > 0 && !cell->hasMine()) {
                cell->revealCell();
            }
        }
    }
}

void Game::revealAll() const {
    for (int32_t row = 0; row < this->rowCount; row++) {
        for (int32_t column = 0; column < this->columnCount; column++) {
            if (Cell *cell = this->cells[row][column].get(); cell != nullptr) {
                cell->revealCell();
            }
        }
    }
}
