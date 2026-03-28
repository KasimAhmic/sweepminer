#include <random>
#include <unordered_set>
#include <queue>
#include <array>

#include <SDL3_image/SDL_image.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "game.hpp"
#include "util.hpp"
#include "mouse.hpp"
#include "pair_hash.hpp"
#include "constants.hpp"
#include "textures.hpp"

typedef std::pair<int8_t, int8_t> Offset;

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

Game::Game(const AppContext &context):
    context(context),
    columns(0),
    rows(0),
    mines(0),
    flags(0),
    state(Game::State::NEW_GAME),
    clock(0),
    timer(std::make_unique<Timer>([this] { this->tick(); }, 1000)),
    resourceContext(std::make_unique<ResourceContext>()) {}

void Game::newGame(const uint8_t columns, const uint8_t rows, const uint16_t mines, const float verticalOffset) {
    this->timer->stop();

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

    this->cells.clear();

    for (uint8_t row = 0; row < this->rows; row++) {
        std::vector<std::unique_ptr<Cell>> cellRow;

        for (uint8_t col = 0; col < this->columns; col++) {
            const SDL_FRect rect{
                static_cast<float>(col * CELL_SIZE + CELL_GRID_OFFSET_X + THICK_BORDER_WIDTH * 2),
                static_cast<float>(row * CELL_SIZE + CELL_GRID_OFFSET_Y + THICK_BORDER_WIDTH * 2) + verticalOffset,
                CELL_SIZE,
                CELL_SIZE
            };
            cellRow.emplace_back(std::make_unique<Cell>(this->context,
                rect,
                id,
                mineCells.contains(id)));
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

                if (newRow < 0 || newRow >= this->rows || newColumn < 0 || newColumn >= this->columns) {
                    continue;
                }

                if (const Cell* cell = this->cells[newRow][newColumn].get(); cell != nullptr && cell->hasMine()) {
                    surroundingMineCount++;
                }
            }

            this->cells[row][column]->setSurroundingMines(surroundingMineCount);
        }
    }

    this->setBoundingBox({
        .x = THICK_BORDER_WIDTH,
        .y = verticalOffset + THICK_BORDER_WIDTH,
        .w = static_cast<float>(columns * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SPACING * 2),
        .h = static_cast<float>(rows * CELL_SIZE + THICK_BORDER_WIDTH * 3 + SCOREBOARD_HEIGHT + SPACING * 3)
    });

    SDL_SetWindowSize(this->context.window,
        static_cast<int32_t>(this->getBoundingBox().w),
        static_cast<int32_t>(this->getBoundingBox().h + verticalOffset));
}

void Game::handleClick() {

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
    SetRenderDrawColor(renderer, COLOR_BUTTON_DEFAULT);
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
        scoreboardBoundingBox.y + scoreboardBoundingBox.h + SPACING,
        static_cast<float>(this->columns) * CELL_SIZE + THICK_BORDER_WIDTH * 2,
        static_cast<float>(this->rows) * CELL_SIZE + THICK_BORDER_WIDTH * 2,
    };

    // this->drawCellGrid(renderer, &cellGridBoundingBox);

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
        this->playSoundEffect(SoundEffect::CLICKED);

        if (cell->getSurroundingMines() > 0) {
            continue;
        }

        for (const auto [deltaColumn, deltaRow]: FOUR_DIR_CELL_OFFSETS) {
            queue.emplace(column + deltaColumn, row + deltaRow);
        }
    }
}

void Game::handleSDLEvent(const SDL_Event& event) {
    if (this->getState() != State::RUNNING && this->getState() != State::NEW_GAME) {
        return;
    }

    const Cell* hitCell = nullptr;

    for (const auto &row : this->cells) {
        bool hit = false;

        for (const auto &cell : row) {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                hit = cell->handleMouseEvent(event.button);

                if (hit) {
                    hitCell = cell.get();
                    break;
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
                hit = cell->handleMouseEvent(event.motion);

                if (hit) {
                    hitCell = cell.get();
                    break;
                }
            }
        }

        if (hit) break;
    }

    if (hitCell != nullptr) {
        this->start();
    }
}

// void Game::handleMouseEvent() {
//     if (this->getState() != Game::State::RUNNING && this->getState() != Game::State::NEW_GAME) {
//         return;
//     }
//
//     OldCell* cell = this->getHoveredCell();
//
//     if (cell == nullptr) {
//         return;
//     }
//
//     if (Mouse::getEvent() == MouseEvent::BUTTON_UP && Mouse::getButton() == MouseButton::LEFT) {
//         this->start();
//
//         const std::optional<std::pair<uint16_t, uint16_t>> clickedCell = cell->reveal();
//
//         if (clickedCell == std::nullopt) {
//             this->end(false);
//             this->playSoundEffect(SoundEffect::EXPLODED);
//             return;
//         }
//
//         this->revealConnectedCells(clickedCell->first, clickedCell->second);
//
//         int32_t revealedCells = 0;
//         int32_t flaggedMines = 0;
//
//         for (const auto &row : this->cells) {
//             for (const auto &rowCell : row) {
//                 if (rowCell->getState() == Cell::State::FLAGGED && rowCell->hasMine()) {
//                     flaggedMines++;
//                 }
//
//                 if (rowCell->getState() == Cell::State::REVEALED) {
//                     revealedCells++;
//                 }
//             }
//         }
//
//         if (this->getColumns() * this->getRows() - revealedCells == flaggedMines) {
//             this->setState(Game::State::VICTORY);
//             this->end(true);
//         }
//
//         return;
//     }
//
//     if (Mouse::getEvent() == MouseEvent::BUTTON_DOWN && Mouse::getButton() == MouseButton::RIGHT) {
//
//         if (cell->getState() == OldCell::OldState::REVEALED) {
//             return;
//         }
//
//         if (cell->getState() == OldCell::OldState::HIDDEN && this->flags > 0) {
//             cell->setState(OldCell::OldState::FLAGGED);
//             this->playSoundEffect(SoundEffect::FLAGGED);
//             this->flags--;
//             return;
//         }
//
//         if (cell->getState() == OldCell::OldState::FLAGGED) {
//             cell->setState(OldCell::OldState::QUESTIONED);
//             this->playSoundEffect(SoundEffect::FLAGGED);
//             this->flags++;
//             return;
//         }
//
//         if (cell->getState() == OldCell::OldState::QUESTIONED) {
//             cell->setState(OldCell::OldState::HIDDEN);
//             this->playSoundEffect(SoundEffect::FLAGGED);
//         }
//     }
// }

void Game::playSoundEffect(SoundEffect soundEffect) const {
    MIX_SetTrackAudio(this->context.audioTrack, this->resourceContext->get(soundEffect));
    MIX_PlayTrack(this->context.audioTrack, 0);
}

void Game::openHighScoreWindow() {
    SDL_Window* window = SDL_CreateWindow("High Scores", 400, 300, SDL_WINDOW_MODAL);

    if (!window) {
        SDL_Log("Couldn't create window: %s\n", SDL_GetError());
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Couldn't create renderer: %s\n", SDL_GetError());
        return;
    }

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetRenderVSync(renderer, 1);

    SDL_ShowWindow(window);
}

// OldCell* Game::getHoveredCell() const {
//     OldCell* cell = nullptr;
//
//     for (const auto &row : this->cells) {
//         for (const auto &rowCell : row) {
//             const SDL_FRect cellRegion{
//                 rowCell->getXPosition(),
//                 rowCell->getYPosition(),
//                 CELL_SIZE,
//                 CELL_SIZE
//             };
//
//             if (Mouse::withinRegion(&cellRegion)) {
//                 cell = rowCell.get();
//                 break;
//             }
//         }
//     }
//
//     return cell;
// }

void Game::loadResources(AppContext* appContext) const {
    this->resourceContext->add(Texture::CELL, Game::loadTexture(appContext, "assets/images/cell.png"));
    this->resourceContext->add(Texture::NUMBERS, Game::loadTexture(appContext, "assets/images/numbers.png"));
    this->resourceContext->add(Texture::SMILEY, Game::loadTexture(appContext, "assets/images/smiley.png"));
    this->resourceContext->add(SoundEffect::CLICKED, Game::loadAudio(appContext, "assets/sounds/click.wav"));
    this->resourceContext->add(SoundEffect::FLAGGED, Game::loadAudio(appContext, "assets/sounds/flag.wav"));
    this->resourceContext->add(SoundEffect::EXPLODED, Game::loadAudio(appContext, "assets/sounds/explosion.wav"));
}

SDL_Texture* Game::loadTexture(const AppContext* appContext, const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(appContext->renderer, path.c_str());

    if (!texture) {
        SDL_Log("Failed to load texture %s: %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    return texture;
}

MIX_Audio* Game::loadAudio(const AppContext* appContext, const std::string& path) {
    MIX_Audio* audio = MIX_LoadAudio(appContext->mixer, path.c_str(), false);

    if (!audio) {
        SDL_Log("Failed to load audio %s: %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }

    return audio;
}

SDL_FRect Game::drawScoreboardBorder(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    const SDL_FRect scoreboardBoundingBox{
        boundingBox->x + SPACING,
        boundingBox->y + SPACING,
        boundingBox->w - SPACING * 2,
        SCOREBOARD_HEIGHT
    };

    DrawBox(renderer,
        scoreboardBoundingBox.x,
        scoreboardBoundingBox.y,
        scoreboardBoundingBox.w,
        scoreboardBoundingBox.h,
        MEDIUM_BORDER_WIDTH,
        COLOR_BUTTON_DEFAULT,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_HIGHLIGHT);

    return scoreboardBoundingBox;
}

void Game::drawFlagCounter(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::NUMBERS);

    DrawBox(renderer,
        boundingBox->x + DISPLAY_OFFSET_X,
        boundingBox->y + DISPLAY_OFFSET_Y,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        THIN_BORDER_WIDTH,
        COLOR_BUTTON_DEFAULT,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_HIGHLIGHT);

    const std::array<uint8_t, 3> flagDigits = Game::getDisplayDigits(this->flags);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * SEGMENT_WIDTH;

        const SDL_FRect dest{
            boundingBox->x + DISPLAY_OFFSET_X + THIN_BORDER_WIDTH + segmentOffset,
            boundingBox->y + DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH,
            SEGMENT_WIDTH,
            SEGMENT_HEIGHT
        };

        SDL_RenderTexture(renderer, texture, TextureOffset::getNumberTextureOffset(flagDigits.at(i)), &dest);
    }
}

void Game::drawButton(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const {
    SDL_Texture* texture = this->resourceContext->get(Texture::SMILEY);

    const SDL_FRect button{
        boundingBox->w / 2 + boundingBox->x - static_cast<float>(BUTTON_WIDTH) / 2,
        boundingBox->h / 2 + boundingBox->y - static_cast<float>(BUTTON_HEIGHT) / 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    // Top left border
    DrawBox(renderer,
        button.x - THIN_BORDER_WIDTH,
        button.y - THIN_BORDER_WIDTH,
        button.w + THIN_BORDER_WIDTH,
        button.h + THIN_BORDER_WIDTH,
        THIN_BORDER_WIDTH,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_SHADOW);

    // Bottom right border
    DrawBox(renderer,
        button.x,
        button.y,
        button.w + THIN_BORDER_WIDTH,
        button.h + THIN_BORDER_WIDTH,
        THIN_BORDER_WIDTH,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_SHADOW);

    const bool isPressed = Mouse::withinRegion(&button) && Mouse::isLeftClicking() && Mouse::eventStartedWithinRegion(&button);

    // Button
    DrawBox(renderer,
        button.x,
        button.y,
        button.w,
        button.h,
        isPressed ? THIN_BORDER_WIDTH : MEDIUM_BORDER_WIDTH,
        COLOR_BUTTON_DEFAULT,
        isPressed ? COLOR_BORDER_SHADOW : COLOR_BORDER_HIGHLIGHT,
        isPressed ? COLOR_BUTTON_DEFAULT : COLOR_BORDER_SHADOW);

    // TODO: This is not quite right, fix it later
    const float smileyOffset = isPressed ? THIN_BORDER_WIDTH : 0.0f;

    const SDL_FRect smiley{
        button.x + THIN_BORDER_WIDTH + smileyOffset,
        button.y + THIN_BORDER_WIDTH + smileyOffset,
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
        boundingBox->y + DISPLAY_OFFSET_Y,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        THIN_BORDER_WIDTH,
        COLOR_BUTTON_DEFAULT,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_HIGHLIGHT);

    const std::array<uint8_t, 3> clockDigits = Game::getDisplayDigits(this->clock);

    for (uint8_t i = 0; i < 3; i++) {
        const float segmentOffset = static_cast<float>(i) * SEGMENT_WIDTH;

        const SDL_FRect dest{
            boundingBox->x + boundingBox->w - (DISPLAY_WIDTH + DISPLAY_OFFSET_X - THIN_BORDER_WIDTH) + segmentOffset,
            boundingBox->y + (DISPLAY_OFFSET_Y + THIN_BORDER_WIDTH),
            SEGMENT_WIDTH,
            SEGMENT_HEIGHT
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
        THICK_BORDER_WIDTH,
        COLOR_BUTTON_DEFAULT,
        COLOR_BORDER_SHADOW,
        COLOR_BORDER_HIGHLIGHT);

    // Cell grid
    for (const auto &row : this->cells) {
        for (const auto &cell : row) {
            cell->render();
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
