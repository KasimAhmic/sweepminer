#pragma once

#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "resource_manager.hpp"

class Context {
public:
    explicit Context(SDL_Window* window,
                     SDL_Renderer* renderer,
                     TTF_TextEngine* textEngine,
                     MIX_Mixer* mixer,
                     MIX_Track* track,
                     float scale,
                     float displayScale);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    [[nodiscard]] SDL_Window* getWindow() const { return this->window; }
    [[nodiscard]] SDL_Renderer* getRenderer() const { return this->renderer; }
    [[nodiscard]] TTF_TextEngine* getTextEngine() const { return this->textEngine; }
    [[nodiscard]] MIX_Mixer* getMixer() const { return this->mixer; }
    [[nodiscard]] MIX_Track* getTrack() const { return this->track; }

    [[nodiscard]] ResourceManager& getResourceManager() const { return *this->resourceManager; }
    [[nodiscard]] float getScale() const { return this->scale; }
    [[nodiscard]] float getDisplayScale() const { return this->displayScale; }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    MIX_Mixer *mixer;
    MIX_Track *track;
    std::unique_ptr<ResourceManager> resourceManager;
    float scale;
    float displayScale;
};
