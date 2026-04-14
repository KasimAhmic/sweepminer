#pragma once

#include <memory>

#include <SDL3/SDL.h>

// TODO: Move elsewhere
enum Menu {
    ID_APP_MENU = 100,
    ID_APP_ABOUT,
    ID_APP_QUIT,

    ID_GAME_MENU,
    ID_GAME_NEW,
    ID_GAME_BEGINNER,
    ID_GAME_INTERMEDIATE,
    ID_GAME_EXPERT,
    ID_GAME_CUSTOM,
    ID_GAME_MARKS,
    ID_GAME_COLOR,
    ID_GAME_SOUND,
    ID_GAME_HIGHSCORES,

    ID_VIEW_MENU,
    ID_VIEW_ZOOM_MENU,
    ID_VIEW_ZOOM_IN,
    ID_VIEW_ZOOM_OUT,
    ID_VIEW_ZOOM_RESET,

    ID_HELP_MENU,
    ID_HELP_GITHUB,
    ID_HELP_REPORT_ISSUE,
    ID_HELP_ABOUT
};

class IMenuBar {
public:
    explicit IMenuBar(SDL_Window* window): window(window) {}
    virtual ~IMenuBar() = default;

    virtual int32_t addMenu(int32_t id, const char* title) = 0;
    virtual int32_t addItem(int32_t id, int32_t parentMenuId, const char* title) = 0;
    virtual int32_t addItem(int32_t id, int32_t parentMenuId, const char* title, const char* icon) = 0;
    virtual int32_t addSubMenu(int32_t id, int32_t parentMenuId, const char* title) = 0;
    virtual int32_t addSubMenu(int32_t id, int32_t parentMenuId, const char* title, const char* icon) = 0;
    virtual void setItemIcon(int32_t id, const char* icon) = 0;
    virtual void addSeparator(int32_t parentMenuId) = 0;

    virtual void handleMenuClick(int32_t itemId) = 0;

protected:
    SDL_Window* window;
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window);
