#pragma once

#include <memory>
#include <vector>

#include "SDL3/SDL.h"

#include "cell.hpp"
#include "resource_context.hpp"
#include "timer.hpp"


enum class Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};

class Game {
public:
    enum class State {
        NEW_GAME,
        RUNNING,
        VICTORY,
        DEFEAT,
    };

    explicit Game(const AppContext &context);
    ~Game() = default;

    void newGame(uint8_t columns, uint8_t rows, uint16_t mines, float verticalOffset);

    void handleClick();

    void newGame(Difficulty difficulty, float verticalOffset);

    void loadResources(AppContext* appContext) const;
    void draw(SDL_Renderer *renderer) const;
    void start();
    void end(bool victory);
    void tick();
    void revealConnectedCells(uint16_t x, uint16_t y);

    void handleSDLEvent(const SDL_Event &event);

    void handleMouseButtonEvent() const;

    void handleMouseMotionEvent() const;

    void handleMouseEvent();
    void playSoundEffect(SoundEffect soundEffect) const;
    void openHighScoreWindow();

    [[nodiscard]] Cell* getHoveredCell() const;
    [[nodiscard]] SDL_FRect getGameSize() const;
    [[nodiscard]] uint8_t getColumns() const { return this->columns; }
    [[nodiscard]] uint8_t getRows() const { return this->rows; }
    [[nodiscard]] uint16_t getMines() const { return this->mines; }
    [[nodiscard]] uint16_t getFlags() const { return this->flags; }
    [[nodiscard]] uint16_t getClock() const { return this->clock; }

    [[nodiscard]] Game::State getState() const { return this->state; }
    void setState(const Game::State state) { this->state = state; }

    [[nodiscard]] SDL_FRect getBoundingBox() const { return this->boundingBox; }
    void setBoundingBox(const SDL_FRect box) { this->boundingBox = box; }

private:
    AppContext context;
    uint8_t columns;
    uint8_t rows;
    uint16_t mines;
    uint16_t flags;
    Game::State state;
    uint16_t clock;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
    std::unique_ptr<Timer> timer;
    std::unique_ptr<ResourceContext> resourceContext;
    SDL_FRect boundingBox{};

    SDL_FRect drawScoreboardBorder(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const;
    void drawFlagCounter(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const;
    void drawButton(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const;
    void drawTimer(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const;
    void drawCellGrid(SDL_Renderer *renderer, const SDL_FRect *boundingBox) const;

    static SDL_Texture* loadTexture(const AppContext* appContext, const std::string& path);
    static MIX_Audio* loadAudio(const AppContext* appContext, const std::string& path);
    [[nodiscard]] static std::array<uint8_t, 3> getDisplayDigits(uint16_t value);
};
