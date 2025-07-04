#include <random>
#include <unordered_set>
#include <queue>

#include "SDL3_image/SDL_image.h"

#include "game.hpp"

#include "util.hpp"
#include "mouse.hpp"
#include "pair_hash.hpp"
#include "constants.hpp"
#include "scaler.hpp"

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
        static_cast<float>(windowWidth),
        static_cast<float>(windowHeight)
    };
    SDL_RenderFillRect(renderer, &rect);

    // Draw the scoreboard
    this->drawScoreboard(renderer, windowWidth);

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
    this->clock = std::min(this->clock + 1, 999);
}

void Game::revealConnectedCells(uint16_t x, uint16_t y) {
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

        if (cell->getState() == State::FLAGGED) {
            this->flags--;
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


void Game::handleMouseEvent() {
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
        const std::optional<std::pair<uint16_t, uint16_t>> clickedCell = cell->reveal();

        if (clickedCell == std::nullopt) {
            return;
        }

        this->revealConnectedCells(clickedCell->first, clickedCell->second);

        return;
    }

    if (Mouse::getEvent() == MouseEvent::BUTTON_DOWN && Mouse::getButton() == MouseButton::RIGHT) {
        if (cell->mark()) {
            if (cell->getState() == State::FLAGGED) {
                this->flags++;
            } else if (cell->getState() == State::QUESTIONED) {
                this->flags--;
            }
        }
    }
}

void Game::loadResources(SDL_Renderer* renderer) const {
    this->resourceContext->add(Texture::CELL, Game::loadTexture(renderer, "assets/images/cell.png"));
    this->resourceContext->add(Texture::NUMBERS, Game::loadTexture(renderer, "assets/images/numbers.png"));
}

SDL_Texture* Game::loadTexture(SDL_Renderer* renderer, const std::string path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

    if (!texture) {
        SDL_Log("Failed to load texture %s: %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    return texture;
}

void Game::drawScoreboard(SDL_Renderer *renderer, const uint32_t windowWidth) const {
    // Scoreboard
    DrawBox(
        renderer,
        Scaler::scaled(SCOREBOARD_OFFSET),
        Scaler::scaled(SCOREBOARD_OFFSET),
        static_cast<float>(windowWidth) - Scaler::scaled(SCOREBOARD_OFFSET * 2) + Scaler::scaled(MEDIUM_BORDER_WIDTH),
        Scaler::scaled(SCOREBOARD_HEIGHT),
        Scaler::scaled(MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    // Flag Counter
    DrawBox(renderer,
        Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_X),
        Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_Y),
        Scaler::scaled(DISPLAY_WIDTH),
        Scaler::scaled(DISPLAY_HEIGHT),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    for (uint8_t i = 0; i < 3; i++) {
        const SDL_FRect dest{
            Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_X + THIN_BORDER_WIDTH) + i * Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, &TextureOffset::NUMBER_ZERO, &dest);
    }

    // Timer
    DrawBox(renderer,
        windowWidth - Scaler::scaled(SCOREBOARD_OFFSET + DISPLAY_OFFSET_X + DISPLAY_WIDTH),
        Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_Y),
        Scaler::scaled(DISPLAY_WIDTH),
        Scaler::scaled(DISPLAY_HEIGHT),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    const std::array clockDigits = {
        static_cast<uint8_t>(this->clock / 100),       // Pull out the first digit
        static_cast<uint8_t>((this->clock / 10) % 10), // Pull out the second digit
        static_cast<uint8_t>(this->clock % 10)         // Pull out the third digit
    };

    static_assert(clockDigits.size() == 3, "Clock digits array must have exactly 3 elements");

    for (uint8_t i = 0; i < 3; i++) {
        const SDL_FRect dest{
            windowWidth - Scaler::scaled(SCOREBOARD_OFFSET + DISPLAY_OFFSET_X + DISPLAY_WIDTH - THIN_BORDER_WIDTH) + i * Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SCOREBOARD_OFFSET + MEDIUM_BORDER_WIDTH + DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, Game::getNumberTextureOffset(clockDigits.at(i)), &dest);
    }
}

const SDL_FRect *Game::getNumberTextureOffset(const uint8_t number) {
    switch (number) {
        case 0: return &TextureOffset::NUMBER_ZERO;
        case 1: return &TextureOffset::NUMBER_ONE;
        case 2: return &TextureOffset::NUMBER_TWO;
        case 3: return &TextureOffset::NUMBER_THREE;
        case 4: return &TextureOffset::NUMBER_FOUR;
        case 5: return &TextureOffset::NUMBER_FIVE;
        case 6: return &TextureOffset::NUMBER_SIX;
        case 7: return &TextureOffset::NUMBER_SEVEN;
        case 8: return &TextureOffset::NUMBER_EIGHT;
        case 9: return &TextureOffset::NUMBER_NINE;
        default: throw std::out_of_range("Number must be between 0 and 9");
    }
}
