#include "cell.hpp"

#include "art.hpp"
#include "game.hpp"

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
    if (this->state == State::HIDDEN) {
        return DrawBox(
            renderer,
            this->xPosition,
            this->yPosition,
            CELL_SIZE,
            CELL_SIZE,
            MEDIUM_BORDER_WIDTH,
            BACKGROUND_COLOR,
            BORDER_HIGHLIGHT_COLOR,
            BORDER_SHADOW_COLOR);
    }

    if (this->state == State::REVEALED) {
        SetRenderDrawColor(renderer, BORDER_SHADOW_COLOR);

        for (uint8_t i = 0; i < SCALE; i++) {
            SDL_RenderLine(renderer, this->xPosition, this->yPosition + i, this->xPosition + CELL_SIZE - 1, this->yPosition + i);
            SDL_RenderLine(renderer, this->xPosition + i, this->yPosition, this->xPosition + i, this->yPosition + CELL_SIZE - 1);
        }


        if (this->hasMine()) {
            return;
        }

        if (this->getSurroundingMines() > 0) {
            SDL_Texture* texture = this->getMineCountTexture();
            const SDL_PropertiesID props = SDL_GetTextureProperties(texture);

            const auto width = static_cast<float>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0));
            const auto height = static_cast<float>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0));
            const auto x = static_cast<float>(this->xPosition) + (CELL_SIZE - width) / 2.0f;
            const auto y = static_cast<float>(this->yPosition) + (CELL_SIZE - height) / 2.0f;

            const SDL_FRect rect{
                .x = x,
                .y = y,
                .w = width,
                .h = height
            };

            SDL_RenderTexture(renderer, texture, nullptr, &rect);
        }
    }
}

SDL_Texture* Cell::getMineCountTexture() const {
    switch (this->getSurroundingMines()) {
        case 1: return this->resourceContext->get(Texture::MINE_ONE);
        case 2: return this->resourceContext->get(Texture::MINE_TWO);
        case 3: return this->resourceContext->get(Texture::MINE_THREE);
        case 4: return this->resourceContext->get(Texture::MINE_FOUR);
        case 5: return this->resourceContext->get(Texture::MINE_FIVE);
        case 6: return this->resourceContext->get(Texture::MINE_SIX);
        case 7: return this->resourceContext->get(Texture::MINE_SEVEN);
        case 8: return this->resourceContext->get(Texture::MINE_EIGHT);
        default: return nullptr;
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

