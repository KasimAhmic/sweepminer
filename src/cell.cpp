#include "cell.hpp"
#include "util.hpp"
#include "game.hpp"
#include "constants.hpp"
#include "mouse.hpp"
#include "textures.hpp"

Cell::Cell(const AppContext &context,
        const uint16_t id,
        const float xPosition,
        const float yPosition,
        const uint8_t column,
        const uint8_t row,
        const bool containsMine,
        const std::shared_ptr<ResourceContext> &resourceContext)
    : context(context),
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
        this->xPosition,
        this->yPosition,
        (CELL_SIZE),
        (CELL_SIZE)
    };

    if (this->state != State::REVEALED) {
        const SDL_FRect cellRect{
            this->xPosition,
            this->yPosition,
            (CELL_SIZE),
            (CELL_SIZE)
        };

        if (Mouse::withinRegion(&cellRect) && Mouse::getState() == MouseState::DOWN && Mouse::getButton() == MouseButton::LEFT) {
            this->drawGrid(renderer);
            return;
        }

        DrawBox(renderer,
            dest.x,
            dest.y,
            CELL_SIZE,
            CELL_SIZE,
            MEDIUM_BORDER_WIDTH,
            BACKGROUND_COLOR,
            BORDER_HIGHLIGHT_COLOR,
            BORDER_SHADOW_COLOR);

        if (this->state == State::HIDDEN) {
            return;
        }
    }

    SDL_Texture* texture = this->resourceContext->get(Texture::CELL);

    if (!texture) {
        return;
    }

    switch (this->state) {
        case State::REVEALED: {
            this->drawGrid(renderer);

            if (this->hasMine()) {
                SDL_RenderTexture(renderer, texture, &TextureOffset::MINE_DETONATED, &dest);
                return;
            }

            if (this->getSurroundingMines() > 0) {
                const SDL_FRect* srcRect = TextureOffset::getCountTextureOffset(this->getSurroundingMines());

                SDL_RenderTexture(renderer, texture, srcRect, &dest);
            }

            break;
        }

        case State::FLAGGED: {
            SDL_RenderTexture(renderer, texture, &TextureOffset::FLAG, &dest);
            break;
        }

        case State::QUESTIONED: {
            SDL_RenderTexture(renderer, texture, &TextureOffset::QUESTION_MARK, &dest);
            break;
        }

        default: {
            break;
        }
    }
}

std::optional<std::pair<uint16_t, uint16_t>> Cell::reveal() {
    if (this->hasMine()) {
        this->state = Cell::State::REVEALED;
        return std::nullopt;
    }

    return std::make_pair(this->getColumn(), this->getRow());
}

void Cell::drawGrid(SDL_Renderer *renderer) const {
    SetRenderDrawColor(renderer, BORDER_SHADOW_COLOR);

    float scaleX, scaleY;

    SDL_GetRenderScale(this->context.renderer, &scaleX, &scaleY);

    for (int32_t i = 0; i < static_cast<int32_t>(scaleX); i++) {
        const auto offset = static_cast<float>(i);

        SDL_RenderLine(renderer,
            this->xPosition,
            this->yPosition + offset,
            this->xPosition + (CELL_SIZE) - 1,
            this->yPosition + offset);

        SDL_RenderLine(renderer,
            this->xPosition + offset,
            this->yPosition,
            this->xPosition + offset,
            this->yPosition + (CELL_SIZE) - 1);
    }
}
