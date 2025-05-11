#pragma once

#include "logger.h"

#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)

inline const logging::Logger* logger = new logging::Logger("Main", logging::Verbose);

constexpr int32_t ERROR_BUFFER_SIZE = 256;

inline std::string DecodeError(DWORD errorCode) {
    const auto outputBuffer = std::make_unique<wchar_t[]>(ERROR_BUFFER_SIZE);

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        outputBuffer.get(),
        ERROR_BUFFER_SIZE,
        nullptr);

    return {outputBuffer.get(), outputBuffer.get() + wcslen(outputBuffer.get())};
}

inline HWND GetClickedControl(HWND parentHandle, const LPARAM longParam) {
    POINT clickLocation = {LOWORD(longParam), HIWORD(longParam)};
    ScreenToClient(parentHandle, &clickLocation);

    return ChildWindowFromPoint(parentHandle, clickLocation);
}
