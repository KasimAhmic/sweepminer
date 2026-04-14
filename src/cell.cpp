#include "cell.hpp"

#include "events.hpp"
#include "textures.hpp"

Cell::Cell(Context *context, const SDL_FRect &rect, const uint8_t row, const uint8_t column, const bool containsMine)
    : Box(context, rect, 3.0f, WHITE, DARK_GREY, GREY),
      Button(rect),
      state(State::HIDDEN),
      row(row),
      column(column),
      surroundingMines(0),
      containsMine(containsMine) {}

Cell::~Cell() = default;

void Cell::render() {
    if (this->getState() == State::HIDDEN || this->getState() == State::FLAGGED || this->getState() == State::QUESTIONED) {
        Box::render();
    }

    if (this->getState() == State::REVEALED && this->getSurroundingMines() > 0) {
        SDL_RenderTexture(
            this->getContext().getRenderer(),
            this->getContext().getResourceManager().getTexture(ResourceManager::Texture::CELL),
            TextureOffset::getCountTextureOffset(this->getSurroundingMines()),
            &Component::getRect());

        return;
    }

    if (this->getState() == State::EXPLODED) {
        SDL_RenderTexture(
            this->getContext().getRenderer(),
            this->getContext().getResourceManager().getTexture(ResourceManager::Texture::CELL),
            &TextureOffset::MINE_DETONATED,
            &Component::getRect());

        return;
    }

    if (this->getState() == State::FLAGGED) {
        SDL_RenderTexture(
            this->getContext().getRenderer(),
            this->getContext().getResourceManager().getTexture(ResourceManager::Texture::CELL),
            &TextureOffset::FLAG,
            &Component::getRect());

        return;
    }

    if (this->getState() == State::QUESTIONED) {
        SDL_RenderTexture(
            this->getContext().getRenderer(),
            this->getContext().getResourceManager().getTexture(ResourceManager::Texture::CELL),
            &TextureOffset::QUESTION_MARK,
            &Component::getRect());
    }
}

void Cell::onMouseOver(const SDL_MouseMotionEvent& event) {
    this->setBackgroundColor(LIGHT_GREY);
}

void Cell::onMouseOut(const SDL_MouseMotionEvent& event) {
    this->setBackgroundColor(GREY);
}

void Cell::onMouseDown(const SDL_MouseButtonEvent& event) {
    if (this->getState() == State::HIDDEN) {
        this->setBackgroundColor(DARK_GREY);
    }
}

void Cell::onMouseUp(const SDL_MouseButtonEvent& event) {
    if (this->getState() != State::REVEALED && this->containsMine && event.button == SDL_BUTTON_LEFT) {
        this->setState(State::EXPLODED);

        MIX_SetTrackAudio(this->getContext().getTrack(), this->getContext().getResourceManager().getSound(ResourceManager::Sound::EXPLODE));
        MIX_PlayTrack(this->getContext().getTrack(), 0);

        SDL_Event loseGameEvent = Events::CreateSweepMinerEvent(Events::LOSE_GAME, 0);
        SDL_PushEvent(&loseGameEvent);

        return;
    }

    if (this->getState() != State::REVEALED && !this->containsMine && event.button == SDL_BUTTON_LEFT) {
        this->setState(State::REVEALED);

        MIX_SetTrackAudio(this->getContext().getTrack(), this->getContext().getResourceManager().getSound(ResourceManager::Sound::CLICK));
        MIX_PlayTrack(this->getContext().getTrack(), 0);

        SDL_Event revealCellEvent = Events::CreateRevealCellEvent(this->row, this->column);
        SDL_PushEvent(&revealCellEvent);

        return;
    }

    if (this->getState() == State::HIDDEN && event.button == SDL_BUTTON_RIGHT) {
        this->setState(State::FLAGGED);

        MIX_SetTrackAudio(this->getContext().getTrack(), this->getContext().getResourceManager().getSound(ResourceManager::Sound::FLAG));
        MIX_PlayTrack(this->getContext().getTrack(), 0);

        return;
    }

    if (this->getState() == State::FLAGGED && event.button == SDL_BUTTON_RIGHT) {
        this->setState(State::QUESTIONED);

        MIX_SetTrackAudio(this->getContext().getTrack(), this->getContext().getResourceManager().getSound(ResourceManager::Sound::FLAG));
        MIX_PlayTrack(this->getContext().getTrack(), 0);

        return;
    }

    if (this->getState() == State::QUESTIONED && event.button == SDL_BUTTON_RIGHT) {
        this->setState(State::HIDDEN);

        MIX_SetTrackAudio(this->getContext().getTrack(), this->getContext().getResourceManager().getSound(ResourceManager::Sound::FLAG));
        MIX_PlayTrack(this->getContext().getTrack(), 0);

        return;
    }
}
