#include "include/framework.h"
#include "include/handles.h"
#include "include/logger.h"
#include "include/util.h"
#include "cell.hpp"

#include <unordered_map>

#include "image.hpp"
#include "include/constants.h"
#include "include/decoder.h"

constexpr int BOX_PADDING = 0;
constexpr int IMAGE_PADDING = 12;
constexpr int BORDER_WIDTH = 4;

const HBRUSH BACKGROUND = CreateSolidBrush(RGB(192, 192, 192));
const HBRUSH BORDER_HIGHLIGHT = CreateSolidBrush(RGB(255, 255, 255));
const HBRUSH BORDER_SHADOW = CreateSolidBrush(RGB(128, 128, 128));
const HBRUSH EXPLOSION = CreateSolidBrush(RGB(255, 0, 0));

const HFONT numberFontHandle = CreateFont(
    -BOX_SIZE / 2,
    0,
    0,
    0,
    FW_BOLD,
    FALSE,
    FALSE,
    FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,
    VARIABLE_PITCH,
    L"Segoe UI");

const std::unordered_map<int8_t, COLORREF> NUMBER_COLORS{
    {1, RGB(0, 0, 255)},
    {2, RGB(0, 128, 0)},
    {3, RGB(255, 0, 0)},
    {4, RGB(0, 0, 128)},
    {5, RGB(128, 0, 0)},
    {6, RGB(0, 128, 128)},
    {7, RGB(0, 0, 0)},
    {8, RGB(128, 128, 128)},
};

Cell::Cell(HINSTANCE instanceHandle, HWND windowHandle, const int id, const int xPosition, const int yPosition, const bool hasMine) {
    this->id = id;
    this->state = DEFAULT;
    this->surroundingMineCount = 0;
    this->hasMine = hasMine;

    this->handle = CreateWindowEx(
        0,
        WC_BUTTON,
        L"",
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        xPosition,
        yPosition,
        BOX_SIZE,
        BOX_SIZE,
        windowHandle,
        reinterpret_cast<HMENU>(this->id),
        instanceHandle,
        nullptr);

    if (!this->handle) {
        logger->fatal("Failed to create the box. Error: ", DecodeError(GetLastError()));
        return;
    }

    SetWindowSubclass(this->handle, BoxProc, id, reinterpret_cast<DWORD_PTR>(this));
}

Cell::~Cell() {
    if (this->handle) {
        RemoveWindowSubclass(this->handle, BoxProc, this->id);
        DestroyWindow(this->handle);
    }
}

LRESULT Cell::BoxProc(HWND windowHandle, UINT message, WPARAM wordParam, LPARAM longParam, UINT_PTR idSubclass, DWORD_PTR boxPointer) {
    logger->verbose("Handle: ", windowHandle, " | Message: ", MESSAGE_MAP[message]);

    const auto box = reinterpret_cast<Cell *>(boxPointer);

    if (!box) {
        logger->warn("Box pointer is null");
        return DefSubclassProc(windowHandle, message, wordParam, longParam);
    }

    switch (message) {
        case WM_LBUTTONDOWN: {
            box->reveal();
            InvalidateRect(windowHandle, nullptr, true);
            break;
        }

        case WM_RBUTTONDOWN: {
            box->mark();
            InvalidateRect(windowHandle, nullptr, true);
            break;
        }

        case WM_PAINT: {
            logger->debug("WM_PAINT");
            PAINTSTRUCT paintStruct{};
            HDC hdc = BeginPaint(windowHandle, &paintStruct);

            RECT boxRect{};
            GetClientRect(windowHandle, &boxRect);

            RECT imageRect{boxRect};
            InflateRect(&imageRect, -IMAGE_PADDING, -IMAGE_PADDING);

            RECT borderRect{boxRect};
            InflateRect(&borderRect, -BORDER_WIDTH / 2, -BORDER_WIDTH / 2);

            boxRect.left += BOX_PADDING;
            boxRect.top += BOX_PADDING;
            boxRect.right -= BOX_PADDING;
            boxRect.bottom -= BOX_PADDING;
            FillRect(hdc, &boxRect, BACKGROUND);

            switch (box->state) {
                case DEFAULT: {
                    box->DrawBorder(hdc, &borderRect);
                    break;
                }

                case FLAGGED: {
                    box->DrawBorder(hdc, &borderRect);
                    DrawImage(hdc, flagImage, IMAGE_PADDING, IMAGE_PADDING, RECT_WIDTH(imageRect), RECT_HEIGHT(imageRect));
                    break;
                }

                case QUESTIONED: {
                    box->DrawBorder(hdc, &borderRect);
                    DrawImage(hdc, questionImage, IMAGE_PADDING, IMAGE_PADDING, RECT_WIDTH(imageRect), RECT_HEIGHT(imageRect));
                    break;
                }

                case REVEALED: {
                    if (box->hasMine) {
                        FillRect(hdc, &boxRect, EXPLOSION);
                        DrawImage(hdc, mineImage, IMAGE_PADDING, IMAGE_PADDING, RECT_WIDTH(imageRect), RECT_HEIGHT(imageRect));
                    } else {
                        SetTextColor(hdc, NUMBER_COLORS.at(4));
                        SetBkMode(hdc, TRANSPARENT);
                        const auto oldFont = static_cast<HFONT>(SelectObject(hdc, numberFontHandle));
                        DrawText(hdc, L"4", 1, &boxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                        SelectObject(hdc, oldFont);
                    }
                    break;
                }
            }

            EndPaint(windowHandle, &paintStruct);

            break;
        }

        default: {
            return DefSubclassProc(windowHandle, message, wordParam, longParam);
        }
    }

    return DefSubclassProc(windowHandle, message, wordParam, longParam);
}

void Cell::mark() {
    if (this->state == REVEALED) {
        return;
    }

    logger->debug("Box::mark");

    this->state = [&] {
        switch (this->state) {
            case DEFAULT: return FLAGGED;
            case FLAGGED: return QUESTIONED;
            case QUESTIONED: return DEFAULT;
            default: return this->state;
        }
    }();

    SetWindowText(this->handle, this->state == FLAGGED ? L"F" : this->state == QUESTIONED ? L"?" : L"");
}

void Cell::reveal() {
    logger->debug("Box::reveal");
    this->state = REVEALED;
    SetWindowText(this->handle, L"R");
    SetWindowPos(this->handle, nullptr, 0, 0, 50, 50, SWP_NOMOVE | SWP_NOZORDER);
}

void Cell::setSurroundingMineCount(const int count) {
    this->surroundingMineCount = count;
}

HWND Cell::getHandle() const {
    return this->handle;
}

void Cell::DrawBorder(HDC hdc, LPRECT rect) {
    LOGBRUSH highlightBrush = { BS_SOLID, RGB(255, 255, 255), 0 };
    HPEN highlightPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, BORDER_WIDTH, &highlightBrush, 0, nullptr);

    LOGBRUSH shadowBrush = { BS_SOLID, RGB(128, 128, 128), 0 };
    HPEN shadowPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, BORDER_WIDTH, &shadowBrush, 0, nullptr);

    HGDIOBJ oldPen = SelectObject(hdc, highlightPen);

    MoveToEx(hdc, rect->left, rect->bottom + BORDER_WIDTH / 2, nullptr);
    LineTo(hdc, rect->left, rect->top);
    LineTo(hdc, rect->right + BORDER_WIDTH / 2, rect->top);

    SelectObject(hdc, shadowPen);
    MoveToEx(hdc, rect->right, rect->top - BORDER_WIDTH / 2, nullptr);
    LineTo(hdc, rect->right, rect->bottom);
    LineTo(hdc, rect->left - BORDER_WIDTH / 2, rect->bottom);

    SelectObject(hdc, oldPen);
    DeleteObject(highlightPen);
    DeleteObject(shadowPen);
}
