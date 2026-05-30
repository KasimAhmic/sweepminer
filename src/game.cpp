#include "game.hpp"

#include <chrono>

#include "box.hpp"
#include "util.hpp"
#include "events.hpp"
#include "profiler.hpp"
#include "menu_bar.hpp"

Game::Game(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, MIX_Mixer* mixer, MIX_Track* track, const float menuBarHeight)
    : context(std::make_unique<Context>(window, renderer, textEngine, mixer, track, SCALE, SDL_GetWindowDisplayScale(window))),
      menuBarHeight(menuBarHeight) {
    this->timer = std::make_unique<Timer>([this] { this->scoreBoard->tick(); }, 1000);
}

Game::~Game() = default;

void Game::init() {
    ResourceManager& manager = this->getContext().getResourceManager();

    manager.loadTexture(ResourceManager::Texture::CELL, "assets/images/cell.png");
    manager.loadTexture(ResourceManager::Texture::NUMBERS, "assets/images/numbers.png");
    manager.loadTexture(ResourceManager::Texture::SMILEY, "assets/images/smiley.png");

    manager.loadSound(ResourceManager::Sound::CLICK, "assets/sounds/click.wav");
    manager.loadSound(ResourceManager::Sound::FLAG, "assets/sounds/flag.wav");
    manager.loadSound(ResourceManager::Sound::EXPLODE, "assets/sounds/explode.wav");

    manager.loadFont(ResourceManager::Font::SOURCE_CODE_PRO, "assets/fonts/SourceCodePro-Medium.ttf", Profiler::FONT_SIZE);

    this->newGame();

    SDL_SetWindowPosition(this->getContext().getWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

// TODO: Maybe I should make this private or handle it all in the constructor and use SDL_Event's everywhere instead
void Game::newGame() {
    this->timer->stop();
    this->background.reset();
    this->scoreBoard.reset();
    this->cellGrid.reset();

    this->setState(State::NEW);

    uint8_t rows{0};
    uint8_t columns{0};
    uint8_t mines{0};

    switch (this->getDifficulty()) {
        case Difficulty::BEGINNER:
            rows = 9;
            columns = 9;
            mines = 10;
            break;
        case Difficulty::INTERMEDIATE:
            rows = 16;
            columns = 16;
            mines = 40;
            break;
        case Difficulty::EXPERT:
            rows = 16;
            columns = 30;
            mines = 99;
            break;
        default:
            std::unreachable();
    }

    const auto [cellGridWidth, cellGridHeight] = CellGrid::getExpectedSize(this->getContext().getScale(), rows, columns);

    const SDL_FRect backgroundRect{
        .x = 0,
        .y = this->menuBarHeight,
        .w = cellGridWidth + PADDING * 2,
        .h = ScoreBoard::HEIGHT + cellGridHeight + PADDING * 3,
    };
    const SDL_FRect scoreBoardRect{
        .x = backgroundRect.x + PADDING,
        .y = backgroundRect.y + PADDING,
        .w = backgroundRect.w - PADDING * 2,
        .h = ScoreBoard::HEIGHT,
    };
    const SDL_FRect cellGridRect{
        .x = scoreBoardRect.x,
        .y = scoreBoardRect.y + scoreBoardRect.h + PADDING,
        .w = cellGridWidth,
        .h = cellGridHeight,
    };

    this->background = std::make_unique<Box>(this->context.get(), backgroundRect, BORDER_WIDTH, WHITE, DARK_GREY, GREY);
    this->scoreBoard = std::make_unique<ScoreBoard>(this->context.get(), scoreBoardRect);
    this->cellGrid = std::make_unique<CellGrid>(this->context.get(), cellGridRect, rows, columns, mines);

    SDL_SetWindowSize(
        this->getContext().getWindow(),
        static_cast<int>(backgroundRect.w),
        static_cast<int>(backgroundRect.h + this->menuBarHeight));
}

void Game::endGame() {
    this->timer->stop();
    this->setState(State::DEFEAT);
}

void Game::start() {
    if (this->getState() == State::NEW) {
        this->setState(State::RUNNING);
        this->timer->start();
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Attempted to start game that is not in NEW state");
    }
}

void Game::handleEvent(const SDL_Event &event) {
    ProfileCall("Score Board Events", this->scoreBoard->handleEvent(event));

    if (event.type == Events::NEW_GAME) {
        this->newGame();
    } else if (event.type == Events::LOSE_GAME) {
        this->endGame();
    } else if (event.type == Events::REVEAL_CELL && this->getState() == State::NEW) {
        this->start();
    }

    ProfileCall("Cell Grid Events", {
        if (this->getState() != State::DEFEAT) {
            this->cellGrid->handleEvent(event);
        }
    });
}

void Game::render(const double deltaTime) const {
    ProfileCall("Background Render", this->background->render());
    ProfileCall("Score Board Render", this->scoreBoard->render());
    ProfileCall("Cell Grid Render", this->cellGrid->render());

#if SWEEPMINER_ENABLE_PROFILER
    ProfileCall("Profiler Render", Profiler::getInstance().render(deltaTime));
#endif
}

#undef ProfileCall
