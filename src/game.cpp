#include "game.hpp"

#include <set>

#include "cell.hpp"
#include "include/constants.h"
#include "include/resource.h"

Game::Game(HINSTANCE instanceHandle, HWND windowHandle) {
    this->instanceHandle = instanceHandle;
    this->windowHandle = windowHandle;
    this->width = 0;
    this->height = 0;
    this->mines = 0;
    this->clock = 0;
    this->flags = 0;
    this->timer = new Timer([this] { this->tick(); }, 1000);
}

Game::~Game() {
    this->timer->stop();
    delete this->timer;
    this->timer = nullptr;
    for (const auto cell : this->cells) {
        delete cell;
    }
    this->cells.clear();
}

RECT Game::start(const Difficulty difficulty) {
    switch (difficulty) {
        case BEGINNER: return start(9, 9, 10);
        case INTERMEDIATE: return start(16, 16, 40);
        case EXPERT: return start(30, 16, 99);
    }

    throw std::invalid_argument("Invalid difficulty");
}

RECT Game::start(const int width, const int height, const int mines) {
    this->width = width;
    this->height = height;
    this->mines = mines;

    const int totalCells = this->width * this->height;

    int id = IDC_FIRST_BOX;
    std::set<int> mineCells;

    while (mineCells.size() < this->mines) {
        mineCells.insert(rand() % totalCells);
    }

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            auto cell = new Cell(this->instanceHandle, this->windowHandle, id, x * BOX_SIZE, y * BOX_SIZE, mineCells.contains(id - IDC_FIRST_BOX));
            this->cells.push_back(cell);
            id++;
        }
    }

    return {0, 0, this->width * BOX_SIZE, this->height * BOX_SIZE};
}

void Game::end() const {
    this->timer->stop();
}

void Game::tick() {
    this->clock++;
}
