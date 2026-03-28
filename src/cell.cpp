#include "cell.hpp"

#include "resource_context.hpp"
#include "textures.hpp"
#include "SDL3_image/SDL_image.h"

void Cell::onMouseOver() {
    this->setBackgroundColor(COLOR_BUTTON_HOVERED);

    if (this->mouseOverCallback != nullptr) {
        this->mouseOverCallback();
    }
}

void Cell::onMouseOut() {
    this->setBackgroundColor(COLOR_BUTTON_DEFAULT);

    if (this->mouseOutCallback != nullptr) {
        this->mouseOutCallback();
    }
}

void Cell::onMouseDown() {
    this->setBackgroundColor(COLOR_BUTTON_ACTIVE);

    if (this->mouseDownCallback != nullptr) {
        this->mouseDownCallback();
    }
}

void Cell::onMouseUp() {
    if (this->getState() == State::HIDDEN) {
        this->setState(State::REVEALED);
    }

    this->setBackgroundColor(COLOR_BUTTON_HOVERED);

    if (this->mouseUpCallback != nullptr) {
        this->mouseUpCallback();
    }
}

void Cell::render() {
    Box::render();

    if (this->getState() == State::REVEALED) {
        SDL_Texture* texture = IMG_LoadTexture(this->context.renderer, "assets/images/cell.png");
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

        if (!texture) {
            return;
        }

        const SDL_FRect dest{
            this->rect.x,
            this->rect.y,
            this->rect.w,
            this->rect.h
        };

        if (this->hasMine()) {
            SDL_RenderTexture(this->context.renderer, texture, &TextureOffset::MINE_DETONATED, &dest);
            return;
        }

        if (this->getSurroundingMines() > 0) {
            const SDL_FRect* srcRect = TextureOffset::getCountTextureOffset(this->getSurroundingMines());

            SDL_RenderTexture(this->context.renderer, texture, srcRect, &dest);
        }

    }
}
