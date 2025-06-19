#pragma once

#include <memory>
#include <gdiplus.h>

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

class ResourceContext {
public:
    ResourceContext() {
        this->images = std::make_shared<ResourceRegistry<std::shared_ptr<Gdiplus::Image>, Image>>();
        this->brushes = std::make_shared<ResourceRegistry<std::shared_ptr<HBRUSH>, Brush>>();
        this->fonts = std::make_shared<ResourceRegistry<std::shared_ptr<HFONT>, Font>>();
    }

    ~ResourceContext() = default;

    void Add(const Image name, const std::shared_ptr<Gdiplus::Image> &image) const {
        this->images->Add(name, image);
    }

    void Add(const Brush name, const std::shared_ptr<HBRUSH> &brush) const {
        this->brushes->Add(name, brush);
    }

    void Add(const Font name, const std::shared_ptr<HFONT> &font) const {
        this->fonts->Add(name, font);
    }

    [[nodiscard]] std::shared_ptr<Gdiplus::Image> Get(const Image image) const {
        return this->images->Get(image);
    }

    [[nodiscard]] HBRUSH Get(const Brush brush) const {
        return *this->brushes->Get(brush);
    }

    [[nodiscard]] HFONT Get(const Font font) const {
        return *this->fonts->Get(font);
    }

private:
    std::shared_ptr<ResourceRegistry<std::shared_ptr<Gdiplus::Image>, Image>> images;
    std::shared_ptr<ResourceRegistry<std::shared_ptr<HBRUSH>, Brush>> brushes;
    std::shared_ptr<ResourceRegistry<std::shared_ptr<HFONT>, Font>> fonts;
};
