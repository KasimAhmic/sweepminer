#include <string>

#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "menu_bar.hpp"

class MenuBar : public IMenuBar {
public:
    explicit MenuBar(SDL_Window* window): IMenuBar(window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

        ImGui::StyleColorsLight();

        ImGui_ImplSDL3_InitForSDLRenderer(window, SDL_GetRenderer(window));
        ImGui_ImplSDLRenderer3_Init(SDL_GetRenderer(window));

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

    }

    void addItem(const int32_t id, const int32_t parentMenuId, const char* title) override {

    }

    void addSubMenu(const int32_t id, const int32_t parentMenuId, const char* title) override {

    }

    void addSeparator(const int32_t parentMenuId) override {

    }

    void handleMenuClick(const int32_t itemId) override {

    }

    void render() override {

    }

private:
    float height;
};

std::unique_ptr<IMenuBar> CreateMenuBar(SDL_Window* window) {
    return std::make_unique<MenuBar>(window);
}
