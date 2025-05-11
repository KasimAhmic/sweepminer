#include <iostream>
#include <map>
#include <set>
#include <string>

#include "./include/framework.h"
#include "./include/logger.h"
#include "game.hpp"
#include "image.hpp"
#include "include/constants.h"
#include "include/decoder.h"
#include "include/handles.h"
#include "include/resource.h"
#include "include/util.h"

ATOM RegisterWindowClass();
BOOL InitInstance(int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

WCHAR windowTitle[MAX_LOAD_STRING];
WCHAR windowClass[MAX_LOAD_STRING];
ULONG_PTR gdiPlusToken;
HINSTANCE instanceHandle;
Game* game;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ const int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    instanceHandle = hInstance;
    std::srand(std::chrono::system_clock::now().time_since_epoch().count());

    INITCOMMONCONTROLSEX icex{};

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;

    if (!InitCommonControlsEx(&icex)) {
        logger->warn("Initialization of common controls failed");
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, nullptr);

    LoadString(instanceHandle, IDS_APP_TITLE, windowTitle, MAX_LOAD_STRING);
    LoadString(instanceHandle, IDC_SWEEP_MINER, windowClass, MAX_LOAD_STRING);
    mineImage = LoadImageFromResource(instanceHandle, IDR_MINE);
    flagImage = LoadImageFromResource(instanceHandle, IDR_FLAG);
    questionImage = LoadImageFromResource(instanceHandle, IDR_QUESTION);

    logger->debug("Window Title: ", windowTitle);
    logger->debug("Window Class: ", windowClass);

    RegisterWindowClass();

    if (!InitInstance(nShowCmd)) {
        logger->fatal("Failed to initialize the application");
        return FALSE;
    }

    const auto acceleratorTableHandle = LoadAccelerators(instanceHandle, MAKEINTRESOURCE(IDC_SWEEP_MINER));

    MSG msg{};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, acceleratorTableHandle, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    Gdiplus::GdiplusShutdown(gdiPlusToken);

    return static_cast<int>(msg.wParam);
}

ATOM RegisterWindowClass() {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instanceHandle;
    wcex.hIcon = LoadIcon(instanceHandle, MAKEINTRESOURCE(IDI_SWEEP_MINER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SWEEP_MINER);
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SWEEP_MINER));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(const int nCmdShow) {
    RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, 0);

    HWND windowHandle = CreateWindowEx(
        0,
        windowClass,
        windowTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        RECT_WIDTH(rect),
        RECT_HEIGHT(rect),
        nullptr,
        nullptr,
        instanceHandle,
        nullptr);

    if (!windowHandle) {
        logger->fatal("Failed to create the main window. Error: ", DecodeError(GetLastError()));
        return FALSE;
    }

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND windowHandle, const UINT message, const WPARAM wordParam, const LPARAM longParam) {
    logger->verbose("Handle: ", windowHandle, " | Message: ", MESSAGE_MAP[message]);

    switch (message) {
        case WM_CREATE: {
            game = new Game(instanceHandle, windowHandle);
            game->start(BEGINNER);

            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wordParam)) {
                default: {
                    return DefWindowProc(windowHandle, message, wordParam, longParam);
                }
            }
        }

        case WM_DESTROY: {
            delete game;

            PostQuitMessage(0);
            break;
        }

        default: {
            return DefWindowProc(windowHandle, message, wordParam, longParam);
        }
    }

    return 0;
}
