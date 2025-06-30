#include <random>
#include <unordered_set>
#include <queue>

#include "SDL3_image/SDL_image.h"

#include "game.hpp"
#include "art.hpp"
#include "mouse.hpp"
#include "pair_hash.hpp"
#include "constants.hpp"
#include "scaler.hpp"

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

Game::Game() {
    this->columns = 0;
    this->rows = 0;
    this->mines = 0;
    this->flags = 0;
    this->running = false;
    this->clock = 0;
    this->timer = std::make_unique<Timer>([this] { this->tick(); }, 1000);
    this->resourceContext = std::make_shared<ResourceContext>();
}

SDL_FRect Game::newGame(const uint8_t columns, const uint8_t rows, const uint16_t mines) {
    this->columns = std::min(columns, MAX_COLUMNS);
    this->rows = std::min(rows, MAX_ROWS);
    this->mines = std::min(mines, MAX_MINES);

    const uint16_t totalCells = this->columns * this->rows;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution distribution(0, totalCells - 1);

    uint16_t id = 0;
    std::unordered_set<uint16_t> mineCells;

    while (mineCells.size() < this->mines) {
        mineCells.insert(distribution(gen));
    }

    for (uint8_t row = 0; row < this->rows; row++) {
        std::vector<std::unique_ptr<Cell>> cellRow;

        for (uint8_t col = 0; col < this->columns; col++) {
            cellRow.emplace_back(std::make_unique<Cell>(
                *this,
                id,
                Scaler::scaled(col * CELL_SIZE + CELL_GRID_OFFSET_X + THICK_BORDER_WIDTH * 2),
                Scaler::scaled(row * CELL_SIZE + CELL_GRID_OFFSET_Y + THICK_BORDER_WIDTH * 2),
                col,
                row,
                mineCells.contains(id),
                this->resourceContext));
            id++;
        }

        this->cells.push_back(std::move(cellRow));
    }

    for (uint16_t row = 0; row < this->rows; row++) {
        for (uint16_t column = 0; column < this->columns; column++) {
            uint8_t surroundingMineCount = 0;

            for (const auto [deltaColumn, deltaRow]: EIGHT_DIR_CELL_OFFSETS) {
                const uint16_t newRow = row + deltaRow;
                const uint16_t newColumn = column + deltaColumn;

                if (newRow < 0 ||
                    newRow >= this->rows ||
                    newColumn < 0 ||
                    newColumn >= this->columns) {
                    continue;
                    }

                if (const Cell *cell = this->cells[newRow][newColumn].get(); cell != nullptr && cell->hasMine()) {
                    surroundingMineCount++;
                }
            }

            this->cells[row][column]->setSurroundingMines(surroundingMineCount);
        }
    }

    return {
        0,
        0,
        static_cast<float>(columns * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SPACING * 2),
        static_cast<float>(rows * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SCOREBOARD_HEIGHT + SPACING * 3)
    };
}

SDL_FRect Game::newGame(const Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::BEGINNER:
            return this->newGame(9, 9, 10);
        case Difficulty::INTERMEDIATE:
            return this->newGame(16, 16, 40);
        case Difficulty::EXPERT:
            return this->newGame(30, 16, 99);
    }

    throw std::invalid_argument("Invalid difficulty");
}

void Game::draw(SDL_Renderer *renderer, const int32_t windowWidth, const int32_t windowHeight) const {
    // Draw the window border
    SetRenderDrawColor(renderer, BACKGROUND_COLOR);
    const SDL_FRect rect = {
        Scaler::scaled(THICK_BORDER_WIDTH),
        Scaler::scaled(THICK_BORDER_WIDTH),
        Scaler::scaled(windowWidth),
        Scaler::scaled(windowHeight)
    };
    SDL_RenderFillRect(renderer, &rect);

    // Draw the scoreboard
    DrawBox(
        renderer,
        Scaler::scaled(SCOREBOARD_OFFSET),
        Scaler::scaled(SCOREBOARD_OFFSET),
        Scaler::scaled(windowWidth - SCOREBOARD_OFFSET * 2 + MEDIUM_BORDER_WIDTH),
        Scaler::scaled(SCOREBOARD_HEIGHT),
        Scaler::scaled(MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    // Draw the cell grid border
    DrawBox(
        renderer,
        Scaler::scaled(THICK_BORDER_WIDTH + CELL_GRID_OFFSET_X),
        Scaler::scaled(THICK_BORDER_WIDTH + CELL_GRID_OFFSET_Y),
        Scaler::scaled(CELL_SIZE * columns + THICK_BORDER_WIDTH * 2),
        Scaler::scaled(CELL_SIZE * rows + THICK_BORDER_WIDTH * 2),
        Scaler::scaled(THICK_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    // Draw the cells
    for (const auto &row : this->cells) {
        for (const auto &cell : row) {
            cell->draw(renderer);
        }
    }
}

void Game::start() const {
    this->timer->start();
}

void Game::end() const {
    this->timer->stop();
}

void Game::tick() {
    this->clock++;
}

void Game::revealConnectedCells(uint16_t x, uint16_t y) const {
    std::queue<std::pair<uint16_t, uint16_t>> queue;
    std::unordered_set<std::pair<uint16_t, uint16_t>, PairHash> visited;

    queue.emplace(x, y);

    while (!queue.empty()) {
        const auto [column, row] = queue.front();
        queue.pop();

        if (column < 0 ||
            column >= this->getColumns() ||
            row < 0 ||
            row >= this->getRows() ||
            visited.contains({column, row})) {
            continue;
        }

        visited.insert({column, row});

        const std::unique_ptr<Cell> &cell = this->cells[row][column];

        if (cell == nullptr || cell->hasMine() || cell->getState() == State::REVEALED) {
            continue;
        }

        cell->revealCell();

        if (cell->getSurroundingMines() > 0) {
            continue;
        }

        for (const auto [deltaColumn, deltaRow]: FOUR_DIR_CELL_OFFSETS) {
            queue.emplace(column + deltaColumn, row + deltaRow);
        }
    }
}


void Game::handleMouseEvent() const {
    const std::optional<std::pair<int32_t, int32_t>> offsets = Mouse::getCellOffsets();

    if (!offsets.has_value()) {
        return;
    }

    const auto [column, row] = offsets.value();

    if (column >= this->columns || row >= this->rows) {
        return;
    }

    const std::unique_ptr<Cell> &cell = this->cells[row][column];

    if (Mouse::getEvent() == MouseEvent::BUTTON_UP && Mouse::getButton() == MouseButton::LEFT) {
        cell->reveal();
        return;
    }

    if (Mouse::getEvent() == MouseEvent::BUTTON_DOWN && Mouse::getButton() == MouseButton::RIGHT) {
        cell->mark();
    }
}

void Game::loadResources(SDL_Renderer* renderer) const {
    this->loadCellCountTexture(renderer);
}

void Game::loadCellCountTexture(SDL_Renderer* renderer) const {
    const std::string texturePath = "assets/images/cell.png";
    SDL_Texture* texture = IMG_LoadTexture(renderer, texturePath.c_str());

    if (!texture) {
        SDL_Log("Failed to load texture %s: %s\n", texturePath.c_str(), SDL_GetError());
        return;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    this->resourceContext->add(Texture::CELL, texture);
}