#pragma once

#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "pair_hash.hpp"

class ResourceManager {
public:
    enum class Texture {
        CELL,
        NUMBERS,
        SMILEY,
    };

    enum class Sound {
        CLICK,
        FLAG,
        EXPLODE,
    };

    enum class Font {
        NF_PIXELS,
        SOURCE_CODE_PRO,
    };

    explicit ResourceManager(SDL_Window* window, SDL_Renderer* renderer, MIX_Mixer* mixer);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    [[nodiscard]] SDL_Texture* getTexture(Texture texture) const;
    [[nodiscard]] MIX_Audio* getSound(Sound sound) const;
    [[nodiscard]] TTF_Font* getFont(Font font, float size) const;

    void loadTexture(Texture texture, const char* path);
    void loadSound(Sound sound, const char* path);
    void loadFont(Font font, const char* path, float size);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    MIX_Mixer* mixer;
    std::unordered_map<Texture, SDL_Texture*> textures;
    std::unordered_map<Sound, MIX_Audio*> sounds;
    std::unordered_map<std::pair<Font, float>, TTF_Font*, PairHash> fonts;
};
