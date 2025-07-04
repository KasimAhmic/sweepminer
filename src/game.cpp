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
#include "textures.hpp"

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

Game::Game():
    columns(0),
    rows(0),
    mines(0),
    flags(0),
    running(false),
    clock(0),
    timer(std::make_unique<Timer>([this] { this->tick(); }, 1000)),
    resourceContext(std::make_shared<ResourceContext>()) {}

SDL_FRect Game::newGame(const uint8_t columns, const uint8_t rows, const uint16_t mines) {
    this->columns = std::min(columns, MAX_COLUMNS);
    this->rows = std::min(rows, MAX_ROWS);
    this->mines = std::min(mines, MAX_MINES);
    this->flags = this->mines;

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
    SetRenderDrawColor(renderer, BACKGROUND_COLOR);
    const SDL_FRect rect = {
        Scaler::scaled(THICK_BORDER_WIDTH),
        Scaler::scaled(THICK_BORDER_WIDTH),
        static_cast<float>(windowWidth),
        static_cast<float>(windowHeight)
    };
    SDL_RenderFillRect(renderer, &rect);

    SDL_FRect scoreboardBoundingBox = this->drawScoreboardBorder(renderer, windowWidth);
    this->drawFlagCounter(renderer, &scoreboardBoundingBox);
    this->drawButton(renderer, &scoreboardBoundingBox);
    this->drawTimer(renderer, &scoreboardBoundingBox);

    SDL_FRect cellGridBoundingBox{};
    this->drawCellGrid(renderer, &scoreboardBoundingBox); // TODO: Change
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

        if (cell == nullptr || cell->hasMine() || cell->getState() == Cell::State::REVEALED) {
            continue;
        }

        if (cell->getState() == Cell::State::FLAGGED) {
            this->flags++;
        }

        cell->setState(Cell::State::REVEALED);

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

        if (cell->getState() == Cell::State::REVEALED) {
            return;
        }

        if (cell->getState() == Cell::State::HIDDEN && this->flags > 0) {
            cell->setState(Cell::State::FLAGGED);
            this->flags--;
            return;
        }

        if (cell->getState() == Cell::State::FLAGGED) {
            cell->setState(Cell::State::QUESTIONED);
            this->flags++;
            return;
        }

        if (cell->getState() == Cell::State::QUESTIONED) {
            cell->setState(Cell::State::HIDDEN);
        }
    }
}

void Game::loadResources(SDL_Renderer* renderer) const {
    this->resourceContext->add(Texture::CELL, Game::loadTexture(renderer, "assets/images/cell.png"));
    this->resourceContext->add(Texture::NUMBERS, Game::loadTexture(renderer, "assets/images/numbers.png"));
    this->resourceContext->add(Texture::SMILEY, Game::loadTexture(renderer, "assets/images/smiley.png"));
}

SDL_Texture* Game::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

    if (!texture) {
        SDL_Log("Failed to load texture %s: %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    return texture;
}

SDL_FRect Game::drawScoreboardBorder(SDL_Renderer *renderer, const uint32_t windowWidth) const {
    const SDL_FRect boundingBox{
        Scaler::scaled(SCOREBOARD_OFFSET),
        Scaler::scaled(SCOREBOARD_OFFSET),
        static_cast<float>(windowWidth) - Scaler::scaled(SCOREBOARD_OFFSET * 2) + Scaler::scaled(MEDIUM_BORDER_WIDTH),
        Scaler::scaled(SCOREBOARD_HEIGHT)
    };

    DrawBox(renderer,
        boundingBox.x,
        boundingBox.y,
        boundingBox.w,
        boundingBox.h,
        Scaler::scaled(MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    return boundingBox;
}

void Game::drawFlagCounter(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    DrawBox(renderer,
        boundingBox->x + Scaler::scaled(DISPLAY_OFFSET_X),
        boundingBox->y + Scaler::scaled(DISPLAY_OFFSET_Y),
        Scaler::scaled(DISPLAY_WIDTH),
        Scaler::scaled(DISPLAY_HEIGHT),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    const std::array<uint8_t, 3> flagDigits = Game::getDisplayDigits(this->flags);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * Scaler::scaled(SEGMENT_WIDTH);

        const SDL_FRect dest{
            boundingBox->x + Scaler::scaled(DISPLAY_OFFSET_X + THIN_BORDER_WIDTH) + segmentOffset,
            boundingBox->y + Scaler::scaled(DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, TextureOffset::getNumberTextureOffset(flagDigits.at(i)), &dest);
    }
}

void Game::drawButton(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::SMILEY);

    const SDL_FRect button{
        boundingBox->w / 2 + boundingBox->x - Scaler::scaled(BUTTON_WIDTH) / 2,
        boundingBox->h / 2 + boundingBox->y - Scaler::scaled(BUTTON_HEIGHT) / 2,
        Scaler::scaled(BUTTON_WIDTH),
        Scaler::scaled(BUTTON_HEIGHT)
    };

    DrawBox(renderer,
        button.x - Scaler::scaled(THIN_BORDER_WIDTH),
        button.y - Scaler::scaled(THIN_BORDER_WIDTH),
        button.w + Scaler::scaled(THIN_BORDER_WIDTH),
        button.h + Scaler::scaled(THIN_BORDER_WIDTH),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR);

    DrawBox(renderer,
        button.x,
        button.y,
        button.w + Scaler::scaled(THIN_BORDER_WIDTH),
        button.h + Scaler::scaled(THIN_BORDER_WIDTH),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR);

    DrawBox(renderer,
        button.x,
        button.y,
        button.w,
        button.h,
        Scaler::scaled(MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_HIGHLIGHT_COLOR,
        BORDER_SHADOW_COLOR);

    const SDL_FRect smiley{
        button.x + Scaler::scaled(THIN_BORDER_WIDTH),
        button.y + Scaler::scaled(THIN_BORDER_WIDTH),
        Scaler::scaled(TextureOffset::SMILEY_DEFAULT.w),
        Scaler::scaled(TextureOffset::SMILEY_DEFAULT.h)
    };

    SDL_RenderTexture(renderer,
        texture,
        Mouse::getState() == MouseState::DOWN && Mouse::getButton() == MouseButton::LEFT ? &TextureOffset::SMILEY_WORRIED : &TextureOffset::SMILEY_DEFAULT,
        &smiley);
}

void Game::drawTimer(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    DrawBox(renderer,
        boundingBox->x + boundingBox->w - Scaler::scaled(DISPLAY_WIDTH + DISPLAY_OFFSET_X),
        boundingBox->y + Scaler::scaled(DISPLAY_OFFSET_Y),
        Scaler::scaled(DISPLAY_WIDTH),
        Scaler::scaled(DISPLAY_HEIGHT),
        Scaler::scaled(THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    const std::array<uint8_t, 3> clockDigits = Game::getDisplayDigits(this->clock);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * Scaler::scaled(SEGMENT_WIDTH);

        const SDL_FRect dest{
            boundingBox->x + boundingBox->w - Scaler::scaled(DISPLAY_WIDTH + DISPLAY_OFFSET_X - THIN_BORDER_WIDTH) + segmentOffset,
            boundingBox->y + Scaler::scaled(DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            Scaler::scaled(SEGMENT_WIDTH),
            Scaler::scaled(SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, TextureOffset::getNumberTextureOffset(clockDigits.at(i)), &dest);
    }
}

void Game::drawCellGrid(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    DrawBox(renderer,
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

std::array<uint8_t, 3> Game::getDisplayDigits(const uint16_t value) {
    return {
        static_cast<uint8_t>(value / 100 % 10), // Pull out the first digit
        static_cast<uint8_t>(value /  10 % 10), // Pull out the second digit
        static_cast<uint8_t>(value /   1 % 10)  // Pull out the third digit
    };
}
