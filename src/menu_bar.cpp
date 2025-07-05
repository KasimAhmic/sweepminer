#include "menu_bar.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "scaler.hpp"

MenuBar::MenuBar(Game *game): game(game), height(0) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    if (ImGui::BeginMainMenuBar()) {
        this->height = ImGui::GetWindowHeight();
        ImGui::EndMainMenuBar();
    }
    ImGui::Render();
}

void MenuBar::draw(SDL_Renderer *renderer) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        this->setHeight(ImGui::GetWindowHeight());

        if (ImGui::BeginMenu("Game")) {
            ImGui::MenuItem("New", "F2");
            ImGui::Separator();
            ImGui::MenuItem("Beginner");
            ImGui::MenuItem("Intermedia");
            ImGui::MenuItem("Expert");
            ImGui::MenuItem("Custom...");
            ImGui::Separator();
            ImGui::MenuItem("Marks (?)");
            ImGui::MenuItem("Color");
            ImGui::MenuItem("Sound");
            ImGui::Separator();
            ImGui::MenuItem("Best Times...");
            ImGui::Separator();
            ImGui::MenuItem("Exit");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu("Scale")) {
                if (ImGui::MenuItem("Small")) {
                    Scaler::setUserScale(1.0f);
                }

                if (ImGui::MenuItem("Medium")) {
                    Scaler::setUserScale(2.0f);
                }

                if (ImGui::MenuItem("Large")) {
                    Scaler::setUserScale(3.0f);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("GitHub");
            ImGui::MenuItem("Report an Issue");
            ImGui::Separator();
            ImGui::MenuItem("About...", "F1");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}
