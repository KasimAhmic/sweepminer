#pragma once

#include "include/framework.h"

enum State {
    DEFAULT,
    FLAGGED,
    QUESTIONED,
    REVEALED,
};

class Cell {
public:
    Cell(HINSTANCE instanceHandle, HWND windowHandle, int id, int xPosition, int yPosition, bool hasMine);
    ~Cell();

    static LRESULT CALLBACK BoxProc(HWND windowHandle, UINT message, WPARAM wordParam, LPARAM longParam, UINT_PTR idSubclass, DWORD_PTR refData);

    void mark();
    void reveal();
    void setSurroundingMineCount(int count);
    [[nodiscard]] HWND getHandle() const;

private:
    int id;
    HWND handle;
    State state;
    int surroundingMineCount;
    bool hasMine;

    static void DrawBorder(HDC hdc, LPRECT rect);
};
