#pragma once

#include <memory>

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "resource_registry.hpp"

enum class Image {
    MINE,
    FLAG,
    QUESTION
};

enum class Brush {
    HIDDEN_BACKGROUND,
    EXPLODED_BACKGROUND,
    BORDER_HIGHLIGHT,
    BORDER_SHADOW,
};

enum class SoundEffect {
    CLICKED,
    FLAGGED,
    EXPLODED
};

enum class Font {
    NUMBER
};

enum class Texture {
    CELL,
    NUMBERS,
    SMILEY,
};

class ResourceContext {
public:
    ResourceContext() {
        this->fonts = std::make_shared<ResourceRegistry<TTF_Font*, Font>>();
        this->textures = std::make_shared<ResourceRegistry<SDL_Texture*, Texture>>();
    }

    ~ResourceContext() = default;

    void add(const Font name, TTF_Font* font) const {
        this->fonts->add(name, font);
    }

    [[nodiscard]] TTF_Font* get(const Font font) const {
        return this->fonts->get(font);
    }

    void add(const Texture name, SDL_Texture* texture) const {
        this->textures->add(name, texture);
    }

    [[nodiscard]] SDL_Texture* get(const Texture texture) const {
        return this->textures->get(texture);
    }

private:
    std::shared_ptr<ResourceRegistry<TTF_Font*, Font>> fonts;
    std::shared_ptr<ResourceRegistry<SDL_Texture*, Texture>> textures;
};
