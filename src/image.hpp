#pragma once

#include <memory>
#include "include/framework.h"

std::unique_ptr<Gdiplus::Image> LoadImageFromResource(HINSTANCE instanceHandle, int resourceId);

void DrawImage(HDC hdc, std::unique_ptr<Gdiplus::Image> &image, int x, int y, int width, int height);
