#pragma once

#include <memory>

#include "box.hpp"
#include "cell_grid.hpp"
#include "context.hpp"
#include "profiler.hpp"
#include "score_board.hpp"
#include "timer.hpp"

class Game {
public:
    enum class State {
        NEW,
        RUNNING,
        VICTORY,
        DEFEAT
    };

    enum class Difficulty {
        BEGINNER,
        INTERMEDIATE,
        EXPERT
    };

    explicit Game(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, MIX_Mixer* mixer, MIX_Track* track, float menuBarHeight);
    ~Game();

    [[nodiscard]] Context& getContext() const { return *this->context; }

    [[nodiscard]] State getState() const { return this->state; }
    void setState(const State newState) { this->state = newState; }

    [[nodiscard]] Difficulty getDifficulty() const { return this->difficulty; }
    void setDifficulty(const Difficulty newDifficulty) { this->difficulty = newDifficulty; }

    void init();
    void newGame();
    void endGame(State endState);
    void start();

    void handleEvent(const SDL_Event &event);
    void render(double deltaTime) const;

private:
    static constexpr float BORDER_WIDTH = 4.0f;
    static constexpr float PADDING = 10.0f;
    static constexpr float SCALE = 1.0f;

    std::unique_ptr<Context> context;
    float menuBarHeight;

    State state = State::NEW;
    Difficulty difficulty = Difficulty::BEGINNER;
    std::unique_ptr<Box> background;
    std::unique_ptr<ScoreBoard> scoreBoard;
    std::unique_ptr<CellGrid> cellGrid;
    std::unique_ptr<Timer> timer;
};
