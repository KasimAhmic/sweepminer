#include "debug_menu.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "mouse.hpp"

DebugMenu::DebugMenu(Game *game): game(game) {}

void DebugMenu::draw(SDL_Renderer *renderer) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");

    {
        ImGui::BeginGroup();
        this->handleRevealAll();
        ImGui::SameLine();
        this->handleRevealNumbers();
        ImGui::SameLine();
        this->handleRevealMines();
        ImGui::EndGroup();
    }

    ImGui::Separator();

    if (ImGui::GetFrameCount() % 10 == 0) {
        if (const Cell* cell = this->game->getHoveredCell(); cell != nullptr) {
            this->cachedHoveredCell = {cell->getColumn(), cell->getRow()};
        }
    }

    const auto [mouseX, mouseY] = Mouse::getPosition();

    {
        ImGui::Text("Columns:      %d", this->game->getColumns());
        ImGui::Text("Rows:         %d", this->game->getRows());
        ImGui::Text("Mine Count:   %d", this->game->getMines());
        ImGui::Text("Flag Count:   %d", this->game->getFlags());
        ImGui::Text("Clock:        %d", this->game->getClock());
    }

    ImGui::Separator();

    {
        ImGui::Text("Mouse Pos:    (X: %d, Y: %d)", mouseX,mouseY);
        ImGui::Text("Hovered Cell: (%d, %d)", this->cachedHoveredCell.first, this->cachedHoveredCell.second);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void DebugMenu::handleRevealAll() {
    if (!ImGui::Button("Reveal All")) {
        return;
    }

    SDL_Log("Reveal All");
}

void DebugMenu::handleRevealNumbers() {
    if (!ImGui::Button("Reveal Numbers")) {
        return;
    }

    SDL_Log("Reveal Numbers");
}

void DebugMenu::handleRevealMines() {
    if (!ImGui::Button("Reveal Mines")) {
        return;
    }

    SDL_Log("Reveal Mines");
}
