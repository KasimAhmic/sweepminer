#pragma once

#include <string>
#include <memory>

#include "SweepMiner/framework.hpp"

#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)

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

inline std::shared_ptr<HBRUSH> MakeSolidBrush(COLORREF color) {
    return {
        new HBRUSH(CreateSolidBrush(color)),
        [](HBRUSH *brush) {
            if (brush) {
                DeleteObject(brush);
            }
        }
    };
}

inline std::shared_ptr<HFONT> MakeFont(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight,
                                       DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet,
                                       DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality,
                                       DWORD iPitchAndFamily, LPCWSTR pszFaceName) {
    return {
        new HFONT(CreateFont(
            cHeight,
            cWidth,
            cEscapement,
            cOrientation,
            cWeight,
            bItalic,
            bUnderline,
            bStrikeOut,
            iCharSet,
            iOutPrecision,
            iClipPrecision,
            iQuality,
            iPitchAndFamily,
            pszFaceName)),
        [](HFONT *font) {
            if (font) {
                DeleteObject(font);
            }
        }
    };
}
