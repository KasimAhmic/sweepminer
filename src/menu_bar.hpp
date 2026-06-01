#pragma once

#include <memory>

#include <SDL3/SDL.h>

class IMenuBar {
public:
    explicit IMenuBar(SDL_Window* window, const uint32_t menuEventId): window(window), menuEventId(menuEventId) {}
    virtual ~IMenuBar() = default;

    virtual void addMenu(int32_t id, const char* title) = 0;
    virtual void addItem(int32_t id, int32_t parentMenuId, const char* title) = 0;
    virtual void addSubMenu(int32_t id, int32_t parentMenuId, const char* title) = 0;
    virtual void addSeparator(int32_t parentMenuId) = 0;

    virtual void render() {}
    [[nodiscard]] virtual bool processMenuEvent(SDL_Event* event) { return false; }

    [[nodiscard]] float getHeight() const { return this->height; }

protected:
    SDL_Window* window{};
    const uint32_t menuEventId{};
    float height{};

    void handleMenuClick(const int32_t itemId) const {
        SDL_Event event{};
        event.type = this->menuEventId;
        event.user.type = this->menuEventId;
        event.user.code = itemId;
        SDL_PushEvent(&event);
    }
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window, uint32_t menuEventId);
