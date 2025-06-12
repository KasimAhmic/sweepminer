#pragma once

#include <memory>
#include "include/framework.h"

std::unique_ptr<Gdiplus::Image> LoadImageFromResource(HINSTANCE instanceHandle, int32_t resourceId);

void DrawImage(HDC hdc, const std::shared_ptr<Gdiplus::Image>& image, int32_t x, int32_t y, int32_t width, int32_t height);
