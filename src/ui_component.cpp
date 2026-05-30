#include "ui_component.hpp"

UiComponent::UiComponent(Context* context, const SDL_FRect& rect)
    : context(context),
      rect(SDL_FRect{
          .x = context->getDisplayScale() * rect.x,
          .y = context->getDisplayScale() * rect.y,
          .w = context->getDisplayScale() * rect.w,
          .h = context->getDisplayScale() * rect.h
      }),
      bounds(rect) {}

UiComponent::~UiComponent() = default;

void UiComponent::render() {
    SDL_SetRenderDrawColor(this->getContext().getRenderer(), 255, 0, 0, 255);
    SDL_RenderFillRect(this->getContext().getRenderer(), &this->getRect());
}
