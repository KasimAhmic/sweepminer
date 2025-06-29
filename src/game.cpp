#include <random>
#include <unordered_set>

#include "game.hpp"

#include <queue>

#include "art.hpp"
#include "pair_hash.hpp"
#include "SDL3_ttf/SDL_ttf.h"

constexpr uint8_t SPACING = 6 * SCALE;
constexpr uint8_t SCOREBOARD_OFFSET = 9 * SCALE;
constexpr uint8_t SCOREBOARD_HEIGHT = 37 * SCALE;
constexpr uint16_t CELL_GRID_OFFSET_X = 6 * SCALE;
constexpr uint16_t CELL_GRID_OFFSET_Y = 49 * SCALE;
constexpr uint8_t MAX_COLUMNS = 30;
constexpr uint8_t MAX_ROWS = 24;
constexpr uint16_t MAX_MINES = 667;

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

constexpr std::array CELL_COLORS = {
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_ONE, "1", SDL_Color(0, 0, 255, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_TWO, "2", SDL_Color(0, 128, 0, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_THREE, "3", SDL_Color(255, 0, 0, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_FOUR, "4", SDL_Color(0, 0, 128, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_FIVE, "5", SDL_Color(128, 0, 0, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_SIX, "6", SDL_Color(0, 128, 128, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_SEVEN, "7", SDL_Color(0, 0, 0, 255)),
    std::tuple<Texture, std::string_view, SDL_Color>(Texture::MINE_EIGHT, "8", SDL_Color(128, 128, 128, 255))
};

Game::Game(const float scale) {
    this->columns = 0;
    this->rows = 0;
    this->mines = 0;
    this->flags = 0;
    this->running = false;
    this->clock = 0;
    this->timer = std::make_unique<Timer>([this] { this->tick(); }, 1000);
    this->resourceContext = std::make_shared<ResourceContext>();
    this->scale = scale;
}

SDL_Rect Game::newGame(const uint8_t columns, const uint8_t rows, const uint16_t mines) {
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
                col * CELL_SIZE + CELL_GRID_OFFSET_X + THICK_BORDER_WIDTH * 2,
                row * CELL_SIZE + CELL_GRID_OFFSET_Y + THICK_BORDER_WIDTH * 2,
                col,
                row,
                mineCells.contains(id),
                this->resourceContext,
                this->scale));
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
        columns * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SPACING * 2,
        rows * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SCOREBOARD_HEIGHT + SPACING * 3
    };
}

SDL_Rect Game::newGame(const Difficulty difficulty) {
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
        THICK_BORDER_WIDTH * this->scale,
        THICK_BORDER_WIDTH * this->scale,
        static_cast<float>(windowWidth) * this->scale,
        static_cast<float>(windowHeight) * this->scale
    };
    SDL_RenderFillRect(renderer, &rect);

    // Draw the scoreboard
    DrawBox(
        renderer,
        SCOREBOARD_OFFSET * this->scale,
        SCOREBOARD_OFFSET * this->scale,
        static_cast<float>(windowWidth - SCOREBOARD_OFFSET * 2 + MEDIUM_BORDER_WIDTH) * this->scale,
        37.0f * SCALE * this->scale,
        MEDIUM_BORDER_WIDTH * this->scale,
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    // Draw the cell grid border
    DrawBox(
        renderer,
        (THICK_BORDER_WIDTH + CELL_GRID_OFFSET_X) * this->scale,
        (THICK_BORDER_WIDTH + CELL_GRID_OFFSET_Y) * this->scale,
        static_cast<float>(CELL_SIZE * columns + THICK_BORDER_WIDTH * 2) * this->scale,
        static_cast<float>(CELL_SIZE * rows + THICK_BORDER_WIDTH * 2) * this->scale,
        THICK_BORDER_WIDTH * this->scale,
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


void Game::handleClick(const SDL_MouseButtonEvent &button) const {
    const auto column = static_cast<uint16_t>((button.x - CELL_GRID_OFFSET_X - THICK_BORDER_WIDTH * 2) / CELL_SIZE);
    const auto row = static_cast<uint16_t>((button.y - CELL_GRID_OFFSET_Y - THICK_BORDER_WIDTH * 2) / CELL_SIZE);

    if (column >= this->columns || row >= this->rows) {
        return;
    }

    const std::unique_ptr<Cell> &cell = this->cells[row][column];

    cell->reveal();
}

void Game::loadResources(SDL_Renderer* renderer) const {
    this->createCellCountFont();
    this->createCellCountTextures(renderer);
}

void Game::createCellCountFont() const {
    const std::string fontPath = "assets/fonts/PublicPixel.ttf";

    TTF_Font *font = TTF_OpenFont(fontPath.c_str(), 1);

    if (!font) {
        SDL_Log("Failed to load font %s: %s\n", fontPath.c_str(), SDL_GetError());
        return;
    }

    this->resourceContext->add(Font::NUMBER, font);
}

void Game::createCellCountTextures(SDL_Renderer* renderer) const {
    for (const auto [name, text, color] : CELL_COLORS) {
        SDL_Surface* surface = TTF_RenderText_Solid(
            this->resourceContext->get(Font::NUMBER),
            text.data(),
            text.length(),
            color);

        this->resourceContext->add(name, SDL_CreateTextureFromSurface(renderer, surface));

        SDL_DestroySurface(surface);
    }
}
