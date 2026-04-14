#include "menu_bar.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#ifdef _WIN32
MenuBar::MenuBar(Game* game): game(game), height(0) {
    const HMENU hMenu = CreateMenu();
    const HMENU hMenuGame = CreateMenu();
    const HMENU hMenuHelp = CreateMenu();

    AppendMenuW(hMenuGame, MF_STRING, 1, L"New");
    AppendMenuW(hMenuGame, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuGame, MF_STRING, 2, L"Beginner");
    AppendMenuW(hMenuGame, MF_STRING, 3, L"Intermediate");
    AppendMenuW(hMenuGame, MF_STRING, 4, L"Expert");
    AppendMenuW(hMenuGame, MF_STRING, 5, L"Custom...");
    AppendMenuW(hMenuGame, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuGame, MF_STRING, 6, L"Marks (?)");
    AppendMenuW(hMenuGame, MF_STRING, 7, L"Color");
    AppendMenuW(hMenuGame, MF_STRING, 8, L"Sound");
    AppendMenuW(hMenuGame, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuGame, MF_STRING, 9, L"Best Times...");
    AppendMenuW(hMenuGame, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuGame, MF_STRING, 10, L"Exit...");

    AppendMenuW(hMenuHelp, MF_STRING, 11, L"GitHub");
    AppendMenuW(hMenuHelp, MF_STRING, 12, L"Report an Issue");
    AppendMenuW(hMenuHelp, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuHelp, MF_STRING, 13, L"About...");

    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuGame), L"Game");
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuHelp), L"Help");

    SetMenu(game->getContext().windowHandle, hMenu);
}

void MenuBar::handleWin32Event(MSG msg) {
    if (msg.message == WM_COMMAND) {
        const int id = LOWORD(msg.wParam);

        switch (id) {
            case 1: // New
                this->game->newGame(Difficulty::BEGINNER, this->getHeight());
                break;

            case 2: // Beginner
                this->game->newGame(Difficulty::BEGINNER, this->getHeight());
                break;

            case 3: // Intermediate
                this->game->newGame(Difficulty::INTERMEDIATE, this->getHeight());
                break;

            case 4: // Expert
                this->game->newGame(Difficulty::EXPERT, this->getHeight());
                break;

            case 5: // Custom...
                break;

            case 6: // Marks (?)
                SDL_Log("Toggling marks");
                break;

            case 7: // Color
                SDL_Log("Toggling color");
                break;

            case 8: // Sound
                SDL_Log("Toggling sound");
                break;

            case 9: // Best Times...
                this->game->openHighScoreWindow();
                break;

            case 10: { // Exit...
                SDL_Event event{ .type = SDL_EVENT_QUIT };
                SDL_PushEvent(&event);
                break;
            }

            case 11: // GitHub
                SDL_OpenURL("");
                break;
        }
    }
}

void MenuBar::draw(SDL_Renderer *renderer) {}

#else

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
        SDL_Event event{ .type = SDL_EVENT_QUIT };
        SDL_PushEvent(&event);
    }
}
#endif
