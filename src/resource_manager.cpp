#include "resource_manager.hpp"

#include <ranges>

#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

ResourceManager::ResourceManager(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer)
    : window(window),
      renderer(renderer),
      mixer(mixer) {}

ResourceManager::~ResourceManager() {
    for (const auto &texture: this->textures | std::views::values) {
        SDL_DestroyTexture(texture);
    }

    for (const auto &sound: this->sounds | std::views::values) {
        MIX_DestroyAudio(sound);
    }

    for (const auto &font: this->fonts | std::views::values) {
        TTF_CloseFont(font);
    }
}

void ResourceManager::loadTexture(const Texture texture, const char *path) {
    if (this->textures.contains(texture)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Texture '%s' already loaded", path);
        return;
    }

    SDL_Texture* sdlTexture = IMG_LoadTexture(this->renderer, path);

    if (!sdlTexture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load texture: %s", path);
        return;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loaded texture: %s", path);

    SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);

    this->textures[texture] = sdlTexture;
}

void ResourceManager::loadSound(const Sound sound, const char *path) {
    if (this->sounds.contains(sound)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Sound '%s' already loaded", path);
        return;
    }

    MIX_Audio* sdlSound = MIX_LoadAudio(this->mixer, path, false);

    if (!sdlSound) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load sound: %s", path);
        return;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loaded sound: %s", path);

    this->sounds[sound] = sdlSound;
}

void ResourceManager::loadFont(Font font, const char *path, const float size) {
    const float scaledSize = size * SDL_GetWindowDisplayScale(this->window);

    if (this->fonts.contains({font, scaledSize})) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Font '%s' of size '%f' already loaded", path, size);
        return;
    }

    TTF_Font* sdlFont = TTF_OpenFont(path, scaledSize);
    if (!sdlFont) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load font: %s", path);
        return;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loaded font: %s @ %.1fpt (%.1fpt)", path, size, scaledSize);

    this->fonts[{font, scaledSize}] = sdlFont;
}

SDL_Texture *ResourceManager::getTexture(const Texture texture) const {
    if (const auto it = this->textures.find(texture); it != this->textures.end()) return it->second;
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture not found: %d", texture);
    return nullptr;
}

MIX_Audio *ResourceManager::getSound(const Sound sound) const {
    if (const auto it = this->sounds.find(sound); it != this->sounds.end()) return it->second;
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Sound not found: %d", sound);
    return nullptr;
}

TTF_Font *ResourceManager::getFont(const Font font, const float size) const {
    const float scaledSize = size * SDL_GetWindowDisplayScale(this->window);

    if (const auto it = this->fonts.find({font, scaledSize}); it != this->fonts.end()) return it->second;
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Font not found: %d @ %.1fpt (%.1fpt)", font, size, scaledSize);
    return nullptr;
}
