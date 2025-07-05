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
    state(Game::State::NEW_GAME),
    clock(0),
    timer(std::make_unique<Timer>([this] { this->tick(); }, 1000)),
    resourceContext(std::make_shared<ResourceContext>()) {}

void Game::newGame(const uint8_t columns, const uint8_t rows, const uint16_t mines, const float verticalOffset) {
    this->columns = std::min(columns, MAX_COLUMNS);
    this->rows = std::min(rows, MAX_ROWS);
    this->mines = std::min(mines, MAX_MINES);
    this->flags = this->mines;
    this->state = Game::State::NEW_GAME;
    this->clock = 0;

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
                (col * CELL_SIZE + CELL_GRID_OFFSET_X + THICK_BORDER_WIDTH * 2),
                (row * CELL_SIZE + CELL_GRID_OFFSET_Y + THICK_BORDER_WIDTH * 2) + verticalOffset,
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

                if (const Cell* cell = this->cells[newRow][newColumn].get(); cell != nullptr && cell->hasMine()) {
                    surroundingMineCount++;
                }
            }

            this->cells[row][column]->setSurroundingMines(surroundingMineCount);
        }
    }

    // TODO: My whole scaling thing is a mess. I need to go through and clean it all up.
    this->setBoundingBox({
        THICK_BORDER_WIDTH,
        verticalOffset + (THICK_BORDER_WIDTH),
        static_cast<float>(columns * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SPACING * 2),
        static_cast<float>(rows * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SCOREBOARD_HEIGHT + SPACING * 3)
    });
}

void Game::newGame(const Difficulty difficulty, const float verticalOffset) {
    switch (difficulty) {
        case Difficulty::BEGINNER:
            return this->newGame(9, 9, 10, verticalOffset);
        case Difficulty::INTERMEDIATE:
            return this->newGame(16, 16, 40, verticalOffset);
        case Difficulty::EXPERT:
            return this->newGame(30, 16, 99, verticalOffset);
    }

    throw std::invalid_argument("Invalid difficulty");
}

void Game::draw(SDL_Renderer *renderer) const {
    SetRenderDrawColor(renderer, BACKGROUND_COLOR);
    const SDL_FRect rect = {
        (this->boundingBox.x),
        this->boundingBox.y, // TODO: Verify the appearance on high DPI displays
        (this->boundingBox.w - THICK_BORDER_WIDTH),
        (this->boundingBox.h - THICK_BORDER_WIDTH)
    };
    SDL_RenderFillRect(renderer, &rect);

    const SDL_FRect scoreboardBoundingBox = this->drawScoreboardBorder(renderer, &rect);
    this->drawFlagCounter(renderer, &scoreboardBoundingBox);
    this->drawButton(renderer, &scoreboardBoundingBox);
    this->drawTimer(renderer, &scoreboardBoundingBox);

    const SDL_FRect cellGridBoundingBox{
        scoreboardBoundingBox.x,
        scoreboardBoundingBox.y + scoreboardBoundingBox.h + (SPACING),
        static_cast<float>(this->columns) * (CELL_SIZE) + (THICK_BORDER_WIDTH * 2),
        static_cast<float>(this->rows) * (CELL_SIZE) + (THICK_BORDER_WIDTH * 2),
    };

    this->drawCellGrid(renderer, &cellGridBoundingBox);
}

void Game::start() {
    this->timer->start();
    this->state = Game::State::RUNNING;
}

void Game::end(const bool victory) {
    this->timer->stop();
    this->state = victory ? Game::State::VICTORY : Game::State::DEFEAT;
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
    if (this->getState() != Game::State::RUNNING && this->getState() != Game::State::NEW_GAME) {
        return;
    }

    Cell* cell = this->getHoveredCell();

    if (cell == nullptr) {
        return;
    }

    if (Mouse::getEvent() == MouseEvent::BUTTON_UP && Mouse::getButton() == MouseButton::LEFT) {
        this->start();

        const std::optional<std::pair<uint16_t, uint16_t>> clickedCell = cell->reveal();

        if (clickedCell == std::nullopt) {
            this->end(false);
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

Cell* Game::getHoveredCell() const {
    Cell* cell = nullptr;

    for (const auto &row : this->cells) {
        for (const auto &rowCell : row) {
            const SDL_FRect cellRegion{
                rowCell->getXPosition(),
                rowCell->getYPosition(),
                (CELL_SIZE),
                (CELL_SIZE)
            };

            if (Mouse::withinRegion(&cellRegion)) {
                cell = rowCell.get();
                break;
            }
        }
    }

    return cell;
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

SDL_FRect Game::drawScoreboardBorder(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    const SDL_FRect scoreboardBoundingBox{
        boundingBox->x + (SPACING),
        boundingBox->y + (SPACING),
        boundingBox->w - (SPACING * 2),
        (SCOREBOARD_HEIGHT)
    };

    DrawBox(renderer,
        scoreboardBoundingBox.x,
        scoreboardBoundingBox.y,
        scoreboardBoundingBox.w,
        scoreboardBoundingBox.h,
        (MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    return scoreboardBoundingBox;
}

void Game::drawFlagCounter(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    DrawBox(renderer,
        boundingBox->x + (DISPLAY_OFFSET_X),
        boundingBox->y + (DISPLAY_OFFSET_Y),
        (DISPLAY_WIDTH),
        (DISPLAY_HEIGHT),
        (THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    const std::array<uint8_t, 3> flagDigits = Game::getDisplayDigits(this->flags);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * (SEGMENT_WIDTH);

        const SDL_FRect dest{
            boundingBox->x + (DISPLAY_OFFSET_X + THIN_BORDER_WIDTH) + segmentOffset,
            boundingBox->y + (DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            (SEGMENT_WIDTH),
            (SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, TextureOffset::getNumberTextureOffset(flagDigits.at(i)), &dest);
    }
}

void Game::drawButton(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::SMILEY);

    const SDL_FRect button{
        boundingBox->w / 2 + boundingBox->x - (BUTTON_WIDTH) / 2,
        boundingBox->h / 2 + boundingBox->y - (BUTTON_HEIGHT) / 2,
        (BUTTON_WIDTH),
        (BUTTON_HEIGHT)
    };

    // Top left border
    DrawBox(renderer,
        button.x - (THIN_BORDER_WIDTH),
        button.y - (THIN_BORDER_WIDTH),
        button.w + (THIN_BORDER_WIDTH),
        button.h + (THIN_BORDER_WIDTH),
        (THIN_BORDER_WIDTH),
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR);

    // Bottom right border
    DrawBox(renderer,
        button.x,
        button.y,
        button.w + (THIN_BORDER_WIDTH),
        button.h + (THIN_BORDER_WIDTH),
        (THIN_BORDER_WIDTH),
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_SHADOW_COLOR);

    const bool isPressed = Mouse::withinRegion(&button) && Mouse::isLeftClicking() && Mouse::eventStartedWithinRegion(&button);

    // Button
    DrawBox(renderer,
        button.x,
        button.y,
        button.w,
        button.h,
        isPressed ? (THIN_BORDER_WIDTH) : (MEDIUM_BORDER_WIDTH),
        BACKGROUND_COLOR,
        isPressed ? BORDER_SHADOW_COLOR : BORDER_HIGHLIGHT_COLOR,
        isPressed ? BACKGROUND_COLOR : BORDER_SHADOW_COLOR);

    // TODO: This is not quite right, fix it later
    const float smileyOffset = isPressed ? (THIN_BORDER_WIDTH) : 0.0f;

    const SDL_FRect smiley{
        button.x + (THIN_BORDER_WIDTH) + smileyOffset,
        button.y + (THIN_BORDER_WIDTH) + smileyOffset,
        (TextureOffset::SMILEY_DEFAULT.w),
        (TextureOffset::SMILEY_DEFAULT.h)
    };

    SDL_FRect textureOffset = TextureOffset::SMILEY_DEFAULT;

    if (this->getState() == Game::State::NEW_GAME || this->getState() == Game::State::RUNNING) {
        textureOffset = TextureOffset::SMILEY_DEFAULT;

        if (Mouse::isLeftClicking() && !Mouse::eventStartedWithinRegion(&button)) {
            textureOffset = TextureOffset::SMILEY_WORRIED;
        }
    } else if (this->getState() == Game::State::VICTORY) {
        textureOffset = TextureOffset::SMILEY_VICTORY;
    } else if (this->getState() == Game::State::DEFEAT) {
        textureOffset = TextureOffset::SMILEY_DEFEAT;
    }

    if (Mouse::isLeftClicking() && Mouse::eventStartedWithinRegion(&button) && Mouse::withinRegion(&button)) {
        textureOffset = TextureOffset::SMILEY_DEFAULT;
    }

    SDL_RenderTexture(renderer, texture, &textureOffset, &smiley);
}

void Game::drawTimer(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    DrawBox(renderer,
        boundingBox->x + boundingBox->w - (DISPLAY_WIDTH + DISPLAY_OFFSET_X),
        boundingBox->y + (DISPLAY_OFFSET_Y),
        (DISPLAY_WIDTH),
        (DISPLAY_HEIGHT),
        (THIN_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    const std::array<uint8_t, 3> clockDigits = Game::getDisplayDigits(this->clock);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * (SEGMENT_WIDTH);

        const SDL_FRect dest{
            boundingBox->x + boundingBox->w - (DISPLAY_WIDTH + DISPLAY_OFFSET_X - THIN_BORDER_WIDTH) + segmentOffset,
            boundingBox->y + (DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            (SEGMENT_WIDTH),
            (SEGMENT_HEIGHT)
        };

        SDL_RenderTexture(renderer, texture, TextureOffset::getNumberTextureOffset(clockDigits.at(i)), &dest);
    }
}

void Game::drawCellGrid(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    // Border
    DrawBox(renderer,
        boundingBox->x,
        boundingBox->y,
        boundingBox->w,
        boundingBox->h,
        (THICK_BORDER_WIDTH),
        BACKGROUND_COLOR,
        BORDER_SHADOW_COLOR,
        BORDER_HIGHLIGHT_COLOR);

    // Cell grid
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
