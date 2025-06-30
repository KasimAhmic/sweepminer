#include "cell.hpp"

#include "art.hpp"
#include "game.hpp"
#include "constants.hpp"
#include "mouse.hpp"
#include "scaler.hpp"

Cell::Cell(Game& game,
           const uint16_t id,
           const uint16_t xPosition,
           const uint16_t yPosition,
           const uint8_t column,
           const uint8_t row,
           const bool containsMine,
           const std::shared_ptr<ResourceContext> &resourceContext)
    : game(game),
      id(id),
      xPosition(xPosition),
      yPosition(yPosition),
      column(column),
      row(row),
      state(State::HIDDEN),
      surroundingMines(0),
      containsMine(containsMine),
      resourceContext(resourceContext) {}

void Cell::draw(SDL_Renderer *renderer) const {
    const SDL_FRect dest{
        .x = static_cast<float>(this->xPosition),
        .y = static_cast<float>(this->yPosition),
        .w = Scaler::scaled(16),
        .h = Scaler::scaled(16)
    };

    if (this->state == State::HIDDEN) {
        int32_t x = -1;
        int32_t y = -1;

        if (const auto offsets = Mouse::getCellOffsets(); offsets.has_value()) {
            std::tie(x, y) = *offsets;
        }

        if (x == this->column && y == this->row && Mouse::getState() == MouseState::DOWN) {
            this->drawGrid(renderer);
            return;
        }

        return DrawBox(renderer,
            dest.x,
            dest.y,
            Scaler::scaled(CELL_SIZE),
            Scaler::scaled(CELL_SIZE),
            Scaler::scaled(MEDIUM_BORDER_WIDTH),
            BACKGROUND_COLOR,
            BORDER_HIGHLIGHT_COLOR,
            BORDER_SHADOW_COLOR);
    }

    SDL_Texture* texture = this->resourceContext->get(Texture::CELL);

    if (!texture) {
        SDL_Log("No texture loaded for a mine count of %d", this->getSurroundingMines());
        return;
    }

    if (this->state == State::REVEALED) {
        this->drawGrid(renderer);

        if (this->hasMine()) {
            SDL_RenderTexture(renderer, texture, &TextureOffset::MINE_DETONATED, &dest);
            return;
        }

        if (this->getSurroundingMines() > 0) {
            const SDL_FRect srcRect = this->getNumberTextureOffset();

            SDL_RenderTexture(renderer, texture, &srcRect, &dest);
        }
    }
}

SDL_FRect Cell::getNumberTextureOffset() const {
    switch (this->getSurroundingMines()) {
        case 1: return TextureOffset::COUNT_ONE;
        case 2: return TextureOffset::COUNT_TWO;
        case 3: return TextureOffset::COUNT_THREE;
        case 4: return TextureOffset::COUNT_FOUR;
        case 5: return TextureOffset::COUNT_FIVE;
        case 6: return TextureOffset::COUNT_SIX;
        case 7: return TextureOffset::COUNT_SEVEN;
        case 8: return TextureOffset::COUNT_EIGHT;
        default: return TextureOffset::NONE;
    }
}

void Cell::reveal() {
    if (this->hasMine()) {
        this->revealCell();
        return;
    }

    this->game.revealConnectedCells(this->getColumn(), this->getRow());
}

void Cell::revealCell() {
    this->state = State::REVEALED;
}

void Cell::drawGrid(SDL_Renderer *renderer) const {
    SetRenderDrawColor(renderer, BORDER_SHADOW_COLOR);

    for (int32_t i = 0; i < Scaler::getTotalScale(); i++) {
        const auto offset = static_cast<float>(i);

        SDL_RenderLine(renderer,
            this->xPosition,
            this->yPosition + offset,
            this->xPosition + Scaler::scaled(CELL_SIZE) - 1,
            this->yPosition + offset);

        SDL_RenderLine(renderer,
            this->xPosition + offset,
            this->yPosition,
            this->xPosition + offset,
            this->yPosition + Scaler::scaled(CELL_SIZE) - 1);
    }
}
