#if SWEEPMINER_PLATFORM_OTHER

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "menu_bar.hpp"

class MenuBar : public IMenuBar {
public:
    explicit MenuBar(SDL_Window* window, const uint32_t menuEventId)
        : IMenuBar(window, menuEventId),
          renderer(SDL_GetRenderer(window)) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

        ImGui::StyleColorsLight();

        ImGui_ImplSDL3_InitForSDLRenderer(window, this->renderer);
        ImGui_ImplSDLRenderer3_Init(this->renderer);

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            this->height = ImGui::GetWindowHeight();
            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
    }

    void addMenu(const int32_t id, const char* title) override {
        const size_t index = rootItems.size();

        this->rootItems.emplace_back(Item{
            .type = ItemType::MENU,
            .id = id,
            .title = title,
            .items = {},
            .itemIndexById = {}
        });

        this->rootIndexById.emplace(id, index);
    }

    void addItem(const int32_t id, const int32_t parentMenuId, const char* title) override {
        Item* parent = findItem(parentMenuId);
        if (!parent) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Couldn't find parent menu ID: %d", parentMenuId);
            return;
        }

        if (parent->type != ItemType::MENU) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Parent ID is not a menu: %d", parentMenuId);
            return;
        }

        if (parent->itemIndexById.contains(id)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Child ID already exists under parent %d: %d", parentMenuId, id);
            return;
        }

        const std::size_t index = parent->items.size();
        parent->items.push_back(Item{
            .type = ItemType::ITEM,
            .id = id,
            .title = title,
            .items = {},
            .itemIndexById = {}
        });
        parent->itemIndexById.emplace(id, index);
    }

    void addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title) override {
        Item* parent = findItem(parentMenuId);
        if (!parent) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Couldn't find parent menu ID: %d", parentMenuId);
            return;
        }

        if (parent->type != ItemType::MENU) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Parent ID is not a menu: %d", parentMenuId);
            return;
        }

        if (parent->itemIndexById.contains(id)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Child ID already exists under parent %d: %d", parentMenuId, id);
            return;
        }

        const std::size_t index = parent->items.size();
        parent->items.push_back(Item{
            .type = ItemType::MENU,
            .id = id,
            .title = title,
            .items = {},
            .itemIndexById = {}
        });
        parent->itemIndexById.emplace(id, index);
    }

    void addSeparator(const int32_t parentMenuId) override {
        Item* parent = findItem(parentMenuId);
        if (!parent) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Couldn't find parent menu ID: %d", parentMenuId);
            return;
        }

        if (parent->type != ItemType::MENU) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Parent ID is not a menu: %d", parentMenuId);
            return;
        }

        parent->items.push_back(Item{
            .type = ItemType::SEPARATOR,
            .id = -1,
            .title = "",
            .items = {},
            .itemIndexById = {}
        });
    }

    bool processMenuEvent(SDL_Event *event) override {
        return ImGui::GetIO().WantCaptureMouse && ImGui_ImplSDL3_ProcessEvent(event);
    }

    void render() override {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            for (const Item& item : this->rootItems) {
                if (item.type == ItemType::MENU) {
                    this->renderMenu(item);
                }
            }

            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), this->renderer);
    }

private:
    enum class ItemType {
        MENU,
        ITEM,
        SEPARATOR
    };

    struct Item {
        ItemType type{};
        int32_t id{-1};
        std::string title;
        std::vector<Item> items;
        std::unordered_map<int32_t, std::size_t> itemIndexById;
    };

    SDL_Renderer* renderer{};

    std::vector<Item> rootItems;
    std::unordered_map<int32_t, std::size_t> rootIndexById;

    Item* findItem(const int32_t id) {
        return findItemRecursive(rootItems, rootIndexById, id);
    }

    static Item* findItemRecursive(
        std::vector<Item>& items,
        std::unordered_map<int32_t, std::size_t>& indexById,
        const int32_t id
    ) {
        if (const auto it = indexById.find(id); it != indexById.end()) {
            return &items[it->second];
        }

        for (Item& item : items) {
            if (item.type != ItemType::MENU) {
                continue;
            }

            if (Item* found = MenuBar::findItemRecursive(item.items, item.itemIndexById, id)) {
                return found;
            }
        }

        return nullptr;
    }

    void renderMenu(const Item& menu) {
        if (ImGui::BeginMenu(menu.title.c_str())) {
            for (const Item& item : menu.items) {
                switch (item.type) {
                    case ItemType::MENU:
                        this->renderMenu(item);
                        break;

                    case ItemType::ITEM:
                        this->renderItem(item);
                        break;

                    case ItemType::SEPARATOR:
                        ImGui::Separator();
                        break;
                }
            }

            ImGui::EndMenu();
        }
    }

    void renderItem(const Item& item) const {
        if (ImGui::MenuItem(item.title.c_str())) {
            this->handleMenuClick(item.id);
        }
    }
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window, const uint32_t menuEventId) {
    return std::make_unique<MenuBar>(window, menuEventId);
}

#endif
