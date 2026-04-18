#include <algorithm>
#include <string>
#include <unordered_map>

#include <Windows.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include "events.hpp"
#include "menu_bar.hpp"

#define TryWithError(expr, message)                                                \
    if (!expr) {                                                                   \
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, message": %lu", GetLastError()); \
        return;                                                                    \
}

#define Try(expr, message)                                  \
    if (!expr) {                                            \
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, message); \
        return;                                             \
    }

class MenuBar : public IMenuBar {
public:
    explicit MenuBar(SDL_Window* window): IMenuBar(window) {
        const SDL_PropertiesID props = SDL_GetWindowProperties(window);

        this->windowHandle = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

        if (!this->windowHandle) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window handle: %lu", GetLastError());
        }

        this->menuHandle = CreateMenu();

        if (!this->menuHandle) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to create menu handle: %lu", GetLastError());
        }

        if (!SetMenu(this->windowHandle, this->menuHandle)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to set menu: %lu", GetLastError());
        }
    }

    void addMenu(const int32_t id, const char* title) override {
        const HMENU menu = CreatePopupMenu();
        TryWithError(menu, "CreatePopupMenu failed");

        this->menuIds[id] = menu;

        const std::wstring wTitle = MenuBar::utf8ToWide(title);

        TryWithError(AppendMenuW(this->menuHandle, MF_POPUP, reinterpret_cast<UINT_PTR>(menu), wTitle.c_str()), "AppendMenuW(top-level) failed");
        TryWithError(DrawMenuBar(this->windowHandle), "DrawMenuBar failed");
    }

    void addItem(const int32_t id, const int32_t parentMenuId, const char* title) override {
        const HMENU parent = this->findMenu(parentMenuId);
        Try(parent, "Parent menu not found");

        const std::wstring wTitle = MenuBar::utf8ToWide(title);

        TryWithError(AppendMenuW(parent, MF_STRING, id, wTitle.c_str()), "AppendMenuW(top-level) failed");
        TryWithError(DrawMenuBar(this->windowHandle), "DrawMenuBar failed");
    }

    void addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title) override {
        const HMENU parent = this->findMenu(parentMenuId);
        Try(parent, "Parent menu not found");

        const HMENU subMenu = CreatePopupMenu();
        TryWithError(subMenu, "CreatePopupMenu failed");

        this->menuIds[id] = subMenu;

        const std::wstring wTitle = utf8ToWide(title);

        TryWithError(AppendMenuW(parent, MF_POPUP, reinterpret_cast<UINT_PTR>(subMenu), wTitle.c_str()), "AppendMenuW(submenu) failed");
        TryWithError(DrawMenuBar(this->windowHandle), "DrawMenuBar failed");
    }

    void addSeparator(const int32_t parentMenuId) override {
        AppendMenuW(this->menuIds[parentMenuId], MF_SEPARATOR, 0, nullptr);
    }

    void handleMenuClick(const int32_t itemId) override {
        SDL_Event event = Events::CreateSweepMinerEvent(Events::MENU_CLICK, itemId);
        SDL_PushEvent(&event);
    }

private:
    HWND windowHandle;
    HMENU menuHandle;
    std::unordered_map<int32_t, HMENU> menuIds;

    [[nodiscard]] static std::wstring utf8ToWide(const char* text) noexcept {
        if (!text) {
            return L"";
        }

        const int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
        if (size <= 0) {
            return L"";
        }

        std::wstring result(size - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), size);
        return result;
    }

    [[nodiscard]] HMENU findMenu(const int32_t id) noexcept {
        const auto it = this->menuIds.find(id);
        if (it == this->menuIds.end()) {
            return nullptr;
        }

        return it->second;
    }
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window) {
    return std::make_unique<MenuBar>(window);
}
