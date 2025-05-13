#include <memory>
#include <gdiplus/gdiplus.h>

#include "resource_registry.hpp"

enum Image {
    MINE,
    FLAG,
    QUESTION
};

struct ResourceContext {
    std::shared_ptr<ResourceRegistry<Gdiplus::Image, Image>> imageRegistry;
};
