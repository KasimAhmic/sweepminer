#pragma once

#include <memory>

#include "framework.h"

// TODO: These cause a segfault on exit cause multiple cells reference them resulting in multiple destructor calls

inline std::unique_ptr<Gdiplus::Image> mineImage;
inline std::unique_ptr<Gdiplus::Image> flagImage;
inline std::unique_ptr<Gdiplus::Image> questionImage;
