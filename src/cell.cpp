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
           const std::shared_ptr<ResourceContext> &resourceContext,
           const float scale)
    : game(game),
      id(id),
      xPosition(xPosition),
      yPosition(yPosition),
      column(column),
      row(row),
      state(State::HIDDEN),
      surroundingMines(0),
      containsMine(containsMine),
      resourceContext(resourceContext),
      scale(scale) {}

void Cell::draw(SDL_Renderer *renderer) const {
    if (this->state == State::HIDDEN) {
        return DrawBox(
            renderer,
            this->xPosition * this->scale,
            this->yPosition * this->scale,
            CELL_SIZE * this->scale,
            CELL_SIZE * this->scale,
            MEDIUM_BORDER_WIDTH * this->scale,
            BACKGROUND_COLOR,
            BORDER_HIGHLIGHT_COLOR,
            BORDER_SHADOW_COLOR);
    }

    if (this->state == State::REVEALED) {
        SetRenderDrawColor(renderer, BORDER_SHADOW_COLOR);

        for (uint8_t i = 0; i < SCALE; i++) {
            SDL_RenderLine(renderer,
                        this->xPosition * this->scale,
                        (this->yPosition + i) * this->scale,
                        (this->xPosition + CELL_SIZE - 1) * this->scale,
                        (this->yPosition + i) * this->scale);

            SDL_RenderLine(renderer,
                        (this->xPosition + i) * this->scale,
                        this->yPosition * this->scale,
                        (this->xPosition + i) * this->scale,
                        (this->yPosition + CELL_SIZE - 1) * this->scale);
        }


        if (this->hasMine()) {
            return;
        }

        if (this->getSurroundingMines() > 0) {
            SDL_Texture* texture = this->getMineCountTexture();

            if (!texture) {
                SDL_Log("No texture loaded for a mine count of %d", this->getSurroundingMines());
                return;
            }

            const SDL_PropertiesID props = SDL_GetTextureProperties(texture);

            const auto width = static_cast<float>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)) * (SCALE * 0.75f);
            const auto height = static_cast<float>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0)) * (SCALE * 0.75f);
            const auto x = static_cast<float>(this->xPosition) + (CELL_SIZE - width) / 2.0f;
            const auto y = static_cast<float>(this->yPosition) + (CELL_SIZE - height) / 2.0f;

            const SDL_FRect rect{
                .x = x * this->scale,
                .y = y * this->scale,
                .w = width * this->scale,
                .h = height * this->scale
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
