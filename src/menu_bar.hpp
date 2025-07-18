#pragma once

#include <SDL3/SDL_render.h>

#include "game.hpp"

class MenuBar {
public:
    explicit MenuBar(Game* game);

    void draw(SDL_Renderer* renderer);

    [[nodiscard]] float getHeight() const { return this->height; }
    void setHeight(const float height) { this->height = height; }

private:
    enum class Toggle {
        MARKS,
        COLOR,
        SOUND
    };

    Game* game;
    float height;

    void handleNewGame(const char* label, const char* shortcut, const Difficulty &difficulty) const;
    void handleToggle(const char* label, const MenuBar::Toggle &toggle) const;
    void handleHighScoreWindow() const;
    void handleExternalLink(const char* label, const char* url) ;
    void handleExit() ;
};
