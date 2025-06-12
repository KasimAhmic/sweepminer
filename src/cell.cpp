#include "include/framework.h"
#include "util.hpp"
#include "cell.hpp"

#include <unordered_map>

#include "include/decoder.h"
#include "include/handles.h"
#include "include/resource.h"
#include "image.hpp"
#include "logger.hpp"

static auto *logger = new logging::Logger("Cell");

const HFONT numberFontHandle = CreateFont(
    -CELL_SIZE / 2,
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

Cell::Cell(
    const std::shared_ptr<ResourceContext> &resourceContext,
    HINSTANCE instanceHandle,
    HWND windowHandle,
    const int32_t id,
    const int32_t xPosition,
    const int32_t yPosition,
    const int32_t column,
    const int32_t row,
    const bool hasMine
) {
    this->resourceContext = resourceContext;
    this->id = id;
    this->state = State::HIDDEN;
    this->surroundingMineCount = 0;
    this->containsMine = hasMine;
    this->column = column;
    this->row = row;

    // TODO: This is dumb, don't create a window for each cell. Go full custom draw.
    this->handle = CreateWindowEx(
        0,
        WC_BUTTON,
        L"",
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        xPosition,
        yPosition,
        CELL_SIZE,
        CELL_SIZE,
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

LRESULT Cell::BoxProc(
    HWND windowHandle,
    UINT message,
    WPARAM wordParam,
    LPARAM longParam,
    UINT_PTR idSubclass,
    DWORD_PTR boxPointer) {
    logger->verbose("Handle: ", windowHandle, " | Message: ", MESSAGE_MAP[message]);

    const auto box = reinterpret_cast<Cell *>(boxPointer);

    if (!box) {
        logger->warn("Cell pointer is null");
        return DefSubclassProc(windowHandle, message, wordParam, longParam);
    }

    switch (message) {
        case WM_LBUTTONDOWN: {
            if (!box->isRevealed()) {
                box->reveal();

                if (box->hasMine()) {
                    PlaySound(MAKEINTRESOURCE(IDR_EXPLOSION), GetModuleHandle(nullptr), SND_RESOURCE | SND_ASYNC);
                } else {
                    PlaySound(MAKEINTRESOURCE(IDR_CLICKED), GetModuleHandle(nullptr), SND_RESOURCE | SND_ASYNC);
                }
            }
            break;
        }

        case WM_RBUTTONDOWN: {
            box->mark();
            PlaySound(MAKEINTRESOURCE(IDR_FLAGGED), GetModuleHandle(nullptr), SND_RESOURCE | SND_ASYNC);
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT paintStruct{};
            HDC hdc = BeginPaint(windowHandle, &paintStruct);

            RECT boxRect{};
            GetClientRect(windowHandle, &boxRect);

            RECT imageRect{boxRect};
            InflateRect(&imageRect, -IMAGE_PADDING, -IMAGE_PADDING);

            RECT borderRect{boxRect};
            InflateRect(&borderRect, -BORDER_WIDTH / 2, -BORDER_WIDTH / 2);

            boxRect.left += CELL_PADDING;
            boxRect.top += CELL_PADDING;
            boxRect.right -= CELL_PADDING;
            boxRect.bottom -= CELL_PADDING;
            FillRect(hdc, &boxRect, box->resourceContext->GetResource(Brush::HIDDEN_BACKGROUND));

            switch (box->state) {
                case State::HIDDEN: {
                    box->DrawBorder(hdc, &borderRect);
                    break;
                }

                case State::FLAGGED: {
                    box->DrawBorder(hdc, &borderRect);
                    DrawImage(
                        hdc,
                        box->resourceContext->GetResource(Image::FLAG),
                        IMAGE_PADDING,
                        IMAGE_PADDING,
                        RECT_WIDTH(imageRect),
                        RECT_HEIGHT(imageRect));
                    break;
                }

                case State::QUESTIONED: {
                    box->DrawBorder(hdc, &borderRect);
                    DrawImage(
                        hdc,
                        box->resourceContext->GetResource(Image::QUESTION),
                        IMAGE_PADDING,
                        IMAGE_PADDING,
                        RECT_WIDTH(imageRect),
                        RECT_HEIGHT(imageRect));
                    break;
                }

                case State::REVEALED: {
                    if (box->containsMine) {
                        FillRect(hdc, &boxRect, box->resourceContext->GetResource(Brush::EXPLODED_BACKGROUND));
                        DrawImage(
                            hdc,
                            box->resourceContext->GetResource(Image::MINE),
                            IMAGE_PADDING,
                            IMAGE_PADDING,
                            RECT_WIDTH(imageRect),
                            RECT_HEIGHT(imageRect));
                    } else {
                        if (box->surroundingMineCount == 0) {
                            break;
                        }

                        SetTextColor(hdc, NUMBER_COLORS.at(box->surroundingMineCount));
                        SetBkMode(hdc, TRANSPARENT);
                        const auto oldFont = static_cast<HFONT>(SelectObject(hdc, numberFontHandle));
                        DrawText(
                            hdc,
                            std::to_wstring(box->surroundingMineCount).c_str(),
                            1,
                            &boxRect,
                            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
    if (this->state == State::REVEALED) {
        return;
    }

    logger->debug("Cell::mark");

    this->state = [&] {
        switch (this->state) {
            case State::HIDDEN: return State::FLAGGED;
            case State::FLAGGED: return State::QUESTIONED;
            case State::QUESTIONED: return State::HIDDEN;
            default: return this->state;
        }
    }();

    InvalidateRect(this->getHandle(), nullptr, true);
}

void Cell::reveal() {
    if (this->hasMine()) {
        this->revealCell();
        return;
    }

    game->revealConnectedEmptyCells(this->column, this->row);
}

void Cell::revealCell() {
    logger->debug("Cell::reveal");
    this->state = State::REVEALED;
    InvalidateRect(this->getHandle(), nullptr, true);
}

void Cell::setSurroundingMineCount(const int32_t count) {
    this->surroundingMineCount = count;
}

void Cell::DrawBorder(HDC hdc, LPRECT rect) {
    LOGBRUSH highlightBrush = {BS_SOLID, RGB(255, 255, 255), 0};
    HPEN highlightPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, BORDER_WIDTH, &highlightBrush, 0, nullptr);

    LOGBRUSH shadowBrush = {BS_SOLID, RGB(128, 128, 128), 0};
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
