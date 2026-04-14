#include "context.hpp"

Context::Context(SDL_Window* window,
                 SDL_Renderer* renderer,
                 TTF_TextEngine* textEngine,
                 MIX_Mixer* mixer,
                 MIX_Track* track,
                 const float scale,
                 const float displayScale)
    : window(window),
      renderer(renderer),
      textEngine(textEngine),
      mixer(mixer),
      track(track),
      scale(scale),
      displayScale(displayScale) {
    this->resourceManager = std::make_unique<ResourceManager>(window, renderer, mixer);
}

Context::~Context() {
    MIX_DestroyTrack(track);
    MIX_DestroyMixer(mixer);
    TTF_DestroyRendererTextEngine(textEngine);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
