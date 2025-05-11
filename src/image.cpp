#include <memory>

#include "include/framework.h"
#include "include/util.h"

std::unique_ptr<Gdiplus::Image> LoadImageFromResource(HINSTANCE instanceHandle, int resourceId) {
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

void DrawImage(HDC hdc, std::unique_ptr<Gdiplus::Image> &image, int x, int y, int width, int height) {
    if (image && image->GetLastStatus() == Gdiplus::Ok) {
        Gdiplus::Graphics graphics(hdc);
        graphics.DrawImage(image.get(), x, y, width, height);
    }
}