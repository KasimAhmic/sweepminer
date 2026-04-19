#include "component.hpp"

Component::Component(Context* context, const SDL_FRect& rect)
    : context(context),
      rect(SDL_FRect{
          .x = context->getDisplayScale() * rect.x,
          .y = context->getDisplayScale() * rect.y,
          .w = context->getDisplayScale() * rect.w,
          .h = context->getDisplayScale() * rect.h
      }),
      bounds(rect) {}

Component::~Component() = default;

void Component::render() {
    SDL_SetRenderDrawColor(this->getContext().getRenderer(), 255, 0, 0, 255);
    SDL_RenderFillRect(this->getContext().getRenderer(), &this->getRect());
}
