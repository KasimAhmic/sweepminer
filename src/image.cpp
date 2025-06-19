#include <memory>

#include "SweepMiner/framework.hpp"

#include "image.hpp"
#include "logging.hpp"
#include "util.hpp"

static auto logger = new logging::Logger("Image");

std::unique_ptr<Gdiplus::Image> LoadImageFromResource(HINSTANCE instanceHandle, int32_t resourceId) {
    HRSRC resourceHandle = FindResource(instanceHandle, MAKEINTRESOURCE(resourceId), RT_RCDATA);

    if (!resourceHandle) {
        logger->error("Failed to find resource. Error: ", DecodeError(GetLastError()));
        return nullptr;
    }

    DWORD size = SizeofResource(instanceHandle, resourceHandle);
    HGLOBAL globalHandle = LoadResource(instanceHandle, resourceHandle);
    void* dataPointer = LockResource(globalHandle);

    if (!dataPointer) {
        logger->error("Failed to load resource. Error: ", DecodeError(GetLastError()));
        return nullptr;
    }

    HGLOBAL copyHandle = GlobalAlloc(GMEM_MOVEABLE, size);

    if (!copyHandle) {
        logger->error("Failed to allocate memory for resource. Error: ", DecodeError(GetLastError()));
        return nullptr;
    }

    void* copyPointer = GlobalLock(copyHandle);
    memcpy_s(copyPointer, size, dataPointer, size);
    GlobalUnlock(copyHandle);

    IStream* stream = nullptr;

    if (FAILED(CreateStreamOnHGlobal(copyHandle, true, &stream))) {
        GlobalFree(copyHandle);
        logger->error("Failed to create stream from resource. Error: ", DecodeError(GetLastError()));
        return nullptr;
    }

    auto image = std::unique_ptr<Gdiplus::Image>(Gdiplus::Image::FromStream(stream));
    stream->Release();

    if (!image || image->GetLastStatus() != Gdiplus::Ok) {
        logger->error("Failed to create image. Error: ", DecodeError(GetLastError()));
        return nullptr;
    }

    return image;
}

void DrawImage(HDC hdc, const std::shared_ptr<Gdiplus::Image>& image, int32_t x, int32_t y, int32_t width, int32_t height) {
    if (image && image->GetLastStatus() == Gdiplus::Ok) {
        Gdiplus::Graphics graphics(hdc);
        graphics.DrawImage(image.get(), x, y, width, height);
    }
}