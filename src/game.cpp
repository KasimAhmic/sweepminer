#include "game.hpp"

#include <chrono>

#include "box.hpp"
#include "util.hpp"
#include "events.hpp"
#include "profiler.hpp"

Game::Game(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, MIX_Mixer* mixer, MIX_Track* track) {
    this->context = std::make_unique<Context>(window, renderer, textEngine, mixer, track, SCALE, SDL_GetWindowDisplayScale(window));
}

Game::~Game() = default;

void Game::init() {
    ResourceManager& manager = this->getContext().getResourceManager();

    manager.loadTexture(ResourceManager::Texture::CELL, "assets/images/cell.png");
    manager.loadTexture(ResourceManager::Texture::NUMBERS, "assets/images/numbers.png");
    manager.loadTexture(ResourceManager::Texture::SMILEY, "assets/images/smiley.png");

    manager.loadSound(ResourceManager::Sound::CLICK, "assets/sounds/click.wav");
    manager.loadSound(ResourceManager::Sound::FLAG, "assets/sounds/flag.wav");
    manager.loadSound(ResourceManager::Sound::EXPLODE, "assets/sounds/explosion.wav");

    manager.loadFont(ResourceManager::Font::SOURCE_CODE_PRO, "assets/fonts/SourceCodePro-Medium.ttf", Profiler::FONT_SIZE);

    this->start();

    SDL_SetWindowPosition(this->getContext().getWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Game::start() {
    this->background.reset();
    this->scoreBoard.reset();
    this->cellGrid.reset();

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
        .y = 0,
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
        static_cast<int>(backgroundRect.h));
}

void Game::handleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_N) {
        this->setDifficulty(static_cast<Difficulty>((static_cast<int>(this->getDifficulty()) + 1) % 3));
        this->start();
    }

    if (event.type == Events::MENU_EVENT) {
        if (event.user.code == ID_APP_MENU) SDL_Log("ID_APP_MENU");
        if (event.user.code == ID_APP_ABOUT) SDL_Log("ID_APP_ABOUT");
        if (event.user.code == ID_APP_QUIT) SDL_Log("ID_APP_QUIT");
        if (event.user.code == ID_GAME_MENU) SDL_Log("ID_GAME_MENU");
        if (event.user.code == ID_GAME_NEW) SDL_Log("ID_GAME_NEW");
        if (event.user.code == ID_GAME_BEGINNER) SDL_Log("ID_GAME_BEGINNER");
        if (event.user.code == ID_GAME_INTERMEDIATE) SDL_Log("ID_GAME_INTERMEDIATE");
        if (event.user.code == ID_GAME_EXPERT) SDL_Log("ID_GAME_EXPORT");
        if (event.user.code == ID_VIEW_MENU) SDL_Log("ID_VIEW_MENU");
        if (event.user.code == ID_VIEW_ZOOM_MENU) SDL_Log("ID_VIEW_ZOOM_MENU");
        if (event.user.code == ID_VIEW_ZOOM_IN) SDL_Log("ID_VIEW_ZOOM_IN");
        if (event.user.code == ID_VIEW_ZOOM_OUT) SDL_Log("ID_VIEW_ZOOM_OUT");
        if (event.user.code == ID_VIEW_ZOOM_RESET) SDL_Log("ID_VIEW_ZOOM_RESET");
        if (event.user.code == ID_HELP_MENU) SDL_Log("ID_HELP_MENU");
        if (event.user.code == ID_HELP_GITHUB) SDL_Log("ID_HELP_GITHUB");
        if (event.user.code == ID_HELP_REPORT_ISSUE) SDL_Log("ID_HELP_REPORT_ISSUE");
        if (event.user.code == ID_HELP_ABOUT) SDL_Log("ID_HELP_ABOUT");
    }

    ProfileCall("Cell Grid Events", this->cellGrid->handleEvent(event));
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
