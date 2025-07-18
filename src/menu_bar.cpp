#include "menu_bar.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

MenuBar::MenuBar(Game* game): game(game), height(0) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    if (ImGui::BeginMainMenuBar()) {
        this->height = ImGui::GetWindowHeight();
        ImGui::EndMainMenuBar();
    }
    ImGui::Render();
}

void MenuBar::draw(SDL_Renderer* renderer) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        this->setHeight(ImGui::GetWindowHeight());

        if (ImGui::BeginMenu("Game")) {
            this->handleNewGame("New", "F2", Difficulty::BEGINNER);

            ImGui::Separator();

            this->handleNewGame("Beginner", nullptr, Difficulty::BEGINNER);
            this->handleNewGame("Intermedia", nullptr, Difficulty::INTERMEDIATE);
            this->handleNewGame("Expert", nullptr, Difficulty::EXPERT);
            ImGui::MenuItem("Custom...");

            ImGui::Separator();

            this->handleToggle("Marks (?)", MenuBar::Toggle::MARKS);
            this->handleToggle("Color", MenuBar::Toggle::COLOR);
            this->handleToggle("Sound", MenuBar::Toggle::SOUND);

            ImGui::Separator();

            this->handleHighScoreWindow();

            ImGui::Separator();

            this->handleExit();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            this->handleExternalLink("GitHub", "https://github.com/KasimAhmic/SweepMiner");
            this->handleExternalLink("Report an Issue", "https://github.com/KasimAhmic/sweepminer/issues");

            ImGui::Separator();

            ImGui::MenuItem("About...", "F1");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void MenuBar::handleNewGame(const char* label, const char* shortcut, const Difficulty &difficulty) const {
    if (ImGui::MenuItem(label, shortcut)) {
        this->game->newGame(difficulty, this->getHeight());
    }
}

void MenuBar::handleToggle(const char* label, const MenuBar::Toggle &toggle) const {
    if (ImGui::MenuItem(label)) {
        switch (toggle) {
            case MenuBar::Toggle::COLOR: {
                SDL_Log("Toggling color");
                break;
            }

            case MenuBar::Toggle::SOUND: {
                SDL_Log("Toggling sound");
                break;
            }

            case MenuBar::Toggle::MARKS: {
                SDL_Log("Toggling marks");
                break;
            }
        }
    }
}

void MenuBar::handleHighScoreWindow() const {
    if (ImGui::MenuItem("Best Times..."))
        this->game->openHighScoreWindow(); {
    }
}

void MenuBar::handleExternalLink(const char* label, const char* url) {
    if (ImGui::MenuItem(label)) {
        SDL_OpenURL(url);
    }
}

void MenuBar::handleExit() {
    if (ImGui::MenuItem("Exit...")) {
        SDL_Quit();
    }
}
