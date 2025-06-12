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

struct ResourceContext {
    std::shared_ptr<ResourceRegistry<std::shared_ptr<Gdiplus::Image>, Image>> imageRegistry;
    std::shared_ptr<ResourceRegistry<std::shared_ptr<HBRUSH>, Brush>> brushRegistry;

    [[nodiscard]] std::shared_ptr<Gdiplus::Image> GetResource(const Image image) const {
        return imageRegistry->GetResource(image);
    }

    [[nodiscard]] HBRUSH GetResource(const Brush brush) const {
        return *brushRegistry->GetResource(brush).get();
    }
};
