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
    const SDL_FRect dest{
        .x = static_cast<float>(this->xPosition) * this->scale,
        .y = static_cast<float>(this->yPosition) * this->scale,
        .w = 16 * SCALE * this->scale,
        .h = 16 * SCALE * this->scale
    };

    if (this->state == State::HIDDEN) {
        return DrawBox(
            renderer,
            dest.x,
            dest.y,
            CELL_SIZE * this->scale,
            CELL_SIZE * this->scale,
            MEDIUM_BORDER_WIDTH * this->scale,
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
        SetRenderDrawColor(renderer, BORDER_SHADOW_COLOR);

        for (uint8_t i = 0; i < SCALE * this->scale; i++) {
            SDL_RenderLine(renderer,
                        this->xPosition * this->scale,
                        this->yPosition * this->scale + i,
                        (this->xPosition + CELL_SIZE) * this->scale - 1,
                        this->yPosition * this->scale + i);

            SDL_RenderLine(renderer,
                        this->xPosition * this->scale + i,
                        this->yPosition * this->scale,
                        this->xPosition * this->scale + i,
                        (this->yPosition + CELL_SIZE) * this->scale - 1);
        }

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
