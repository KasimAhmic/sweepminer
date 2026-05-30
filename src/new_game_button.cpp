#include "new_game_button.hpp"

#include "events.hpp"
#include "textures.hpp"

NewGameButton::NewGameButton(Context *context, const SDL_FRect &rect)
    : Box(context, rect, BORDER_WIDTH, WHITE, DARK_GREY, GREY),
      Button(rect) {}

NewGameButton::~NewGameButton() = default;

void NewGameButton::render() {
    Box::render();

    SDL_FRect srcRect;

    switch (this->getState()) {
        case State::WORRIED:
            srcRect = TextureOffset::SMILEY_WORRIED;
            break;
        case State::DEFEAT:
            srcRect = TextureOffset::SMILEY_DEFEAT;
            break;
        case State::VICTORY:
            srcRect = TextureOffset::SMILEY_VICTORY;
            break;
        default:
            srcRect = TextureOffset::SMILEY_DEFAULT;
    }

    SDL_FRect destRect{
        .x = Box::getRect().x + PADDING,
        .y = Box::getRect().y + PADDING,
        .w = TextureOffset::SMILEY_DEFAULT.w,
        .h = TextureOffset::SMILEY_DEFAULT.h,
    };

    if (this->getState() == State::PRESSED) {
        destRect.x += 1.0f;
        destRect.y += 1.0f;
    }

    SDL_RenderTexture(
        this->getContext().getRenderer(),
        this->getContext().getResourceManager().getTexture(ResourceManager::Texture::SMILEY),
        &srcRect,
        &destRect);
}

void NewGameButton::onMouseOver(const SDL_MouseMotionEvent& event) {
    (void)event;

    this->setBackgroundColor(LIGHT_GREY);
}

void NewGameButton::onMouseOut(const SDL_MouseMotionEvent& event) {
    (void)event;

    if (this->getState() != State::DEFEAT) {
        this->setState(State::DEFAULT);
    }

    this->setBackgroundColor(GREY);
    this->setBorderPrimaryColor(WHITE);
    this->setBorderSecondaryColor(DARK_GREY);
}

void NewGameButton::onMouseDown(const SDL_MouseButtonEvent& event) {
    (void)event;

    if (this->getState() != State::DEFEAT) {
        this->setState(State::PRESSED);
    }

    this->setBackgroundColor(LIGHT_GREY);
    this->setBorderPrimaryColor(DARK_GREY);
    this->setBorderSecondaryColor(WHITE);
}

void NewGameButton::onMouseUp(const SDL_MouseButtonEvent& event) {
    (void)event;

    if (this->getState() != State::DEFEAT) {
        this->setState(State::DEFAULT);
    }

    this->setBackgroundColor(LIGHT_GREY);
    this->setBorderPrimaryColor(WHITE);
    this->setBorderSecondaryColor(DARK_GREY);

    SDL_Event newGameEvent = Events::CreateSweepMinerEvent(Events::NEW_GAME, 0);
    SDL_PushEvent(&newGameEvent);
}
