#include "OldCell.hpp"
#include "util.hpp"
#include "game.hpp"
#include "constants.hpp"
#include "mouse.hpp"
#include "textures.hpp"

OldCell::OldCell(const AppContext &context,
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
      state(OldState::HIDDEN),
      surroundingMines(0),
      containsMine(containsMine),
      resourceContext(resourceContext) {}

void OldCell::draw(SDL_Renderer *renderer) const {
    const SDL_FRect dest{
        this->xPosition,
        this->yPosition,
        (CELL_SIZE),
        (CELL_SIZE)
    };

    if (this->state != OldState::REVEALED) {
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
            COLOR_BUTTON_BG,
            BORDER_HIGHLIGHT_COLOR,
            BORDER_SHADOW_COLOR);

        if (this->state == OldState::HIDDEN) {
            return;
        }
    }

    SDL_Texture* texture = this->resourceContext->get(Texture::CELL);

    if (!texture) {
        return;
    }

    switch (this->state) {
        case OldState::REVEALED: {
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

        case OldState::FLAGGED: {
            SDL_RenderTexture(renderer, texture, &TextureOffset::FLAG, &dest);
            break;
        }

        case OldState::QUESTIONED: {
            SDL_RenderTexture(renderer, texture, &TextureOffset::QUESTION_MARK, &dest);
            break;
        }

        default: {
            break;
        }
    }
}

std::optional<std::pair<uint16_t, uint16_t>> OldCell::reveal() {
    if (this->hasMine()) {
        this->state = OldCell::OldState::REVEALED;
        return std::nullopt;
    }

    return std::make_pair(this->getColumn(), this->getRow());
}

void OldCell::drawGrid(SDL_Renderer *renderer) const {
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
