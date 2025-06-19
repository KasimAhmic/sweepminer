#include <iostream>
#include <string>

#include "SweepMiner/framework.h"
#include "SweepMiner/decoder.h"
#include "SweepMiner/resource.h"

#include "logging.hpp"
#include "game.hpp"
#include "image.hpp"
#include "util.hpp"

constexpr int32_t MAX_LOAD_STRING = 128;

static auto *logger = new logging::Logger("Main");

std::unique_ptr<Game> game;

ATOM RegisterWindowClass();
BOOL InitInstance(int32_t);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

WCHAR windowTitle[MAX_LOAD_STRING];
WCHAR windowClass[MAX_LOAD_STRING];
WCHAR githubUrl[MAX_LOAD_STRING];
WCHAR githubIssuesUrl[MAX_LOAD_STRING];

ULONG_PTR gdiPlusToken;
HINSTANCE instanceHandle;

int32_t APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ const int32_t nShowCmd) {
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
    LoadString(instanceHandle, IDS_GITHUB_URL, githubUrl, MAX_LOAD_STRING);
    LoadString(instanceHandle, IDS_GITHUB_ISSUES_URL, githubIssuesUrl, MAX_LOAD_STRING);

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

    game.reset();
    Gdiplus::GdiplusShutdown(gdiPlusToken);

    return static_cast<int32_t>(msg.wParam);
}

ATOM RegisterWindowClass() {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instanceHandle;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SWEEP_MINER);
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(const int32_t nCmdShow) {
    HWND windowHandle = CreateWindowEx(
        0,
        windowClass,
        windowTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        instanceHandle,
        nullptr);

    if (!windowHandle) {
        logger->fatal("Failed to create the main window. Error: ", DecodeError(GetLastError()));
        return false;
    }

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);

    return true;
}

void StartNewGame(HWND windowHandle, const Difficulty difficulty) {
    if (game) {
        game.reset();
    }

    game = std::make_unique<Game>(instanceHandle, windowHandle);

    RECT rect = game->start(difficulty);

    const LONG_PTR style = GetWindowLongPtr(windowHandle, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtr(windowHandle, GWL_EXSTYLE);

    AdjustWindowRectEx(&rect, style, true, exStyle);

    SetWindowPos(windowHandle, nullptr, 0, 0, RECT_WIDTH(rect), RECT_HEIGHT(rect), SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT CALLBACK WndProc(HWND windowHandle, const UINT message, const WPARAM wordParam, const LPARAM longParam) {
    logger->verbose("Handle: ", windowHandle, " | Message: ", MESSAGE_MAP[message]);

    switch (message) {
        case WM_CREATE: {
            StartNewGame(windowHandle, BEGINNER);
            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wordParam)) {
                case IDM_GAME_NEW: {
                    logger->debug("IDM_GAME_NEW");
                    StartNewGame(windowHandle, BEGINNER);
                    break;
                }

                case IDM_GAME_BEGINNER: {
                    logger->debug("IDM_GAME_BEGINNER");
                    StartNewGame(windowHandle, BEGINNER);
                    break;
                }

                case IDM_GAME_INTERMEDIATE: {
                    logger->debug("IDM_GAME_INTERMEDIATE");
                    StartNewGame(windowHandle, INTERMEDIATE);
                    break;
                }

                case IDM_GAME_EXPERT: {
                    logger->debug("IDM_GAME_EXPERT");
                    StartNewGame(windowHandle, EXPERT);
                    break;
                }

                case IDM_GAME_CUSTOM: {
                    logger->debug("IDM_GAME_CUSTOM");
                    break;
                }

                case IDM_GAME_MARKS: {
                    logger->debug("IDM_GAME_MARKS");
                    break;
                }

                case IDM_GAME_COLOR: {
                    logger->debug("IDM_GAME_COLOR");
                    break;
                }

                case IDM_GAME_SOUND: {
                    logger->debug("IDM_GAME_SOUND");
                    break;
                }

                case IDM_GAME_BEST_TIMES: {
                    logger->debug("IDM_GAME_BEST_TIMES");
                    break;
                }

                case IDM_GAME_EXIT: {
                    logger->debug("IDM_GAME_EXIT");
                    SendMessage(windowHandle, WM_CLOSE, 0, 0);
                    break;
                }

                case IDM_HELP_GITHUB: {
                    logger->debug("IDM_HELP_GITHUB");
                    ShellExecute(nullptr, L"open", githubUrl, nullptr, nullptr, SW_SHOWNORMAL);
                    break;
                }

                case IDM_HELP_REPORT_ISSUE: {
                    logger->debug("IDM_HELP_REPORT_ISSUE");
                    ShellExecute(nullptr, L"open", githubIssuesUrl, nullptr, nullptr, SW_SHOWNORMAL);
                    break;
                }

                case IDM_HELP_ABOUT: {
                    logger->debug("IDM_HELP_ABOUT");
                    break;
                }

                case IDM_DEBUG_SHOW_MINES: {
                    logger->debug("IDM_DEBUG_SHOW_MINES");
                    game->showMines();
                    break;
                }

                case IDM_DEBUG_SHOW_COUNTS: {
                    logger->debug("IDM_DEBUG_SHOW_COUNTS");
                    game->showCounts();
                    break;
                }

                case IDM_DEBUG_REVEAL_ALL: {
                    logger->debug("IDM_DEBUG_REVEAL_ALL");
                    game->revealAll();
                    break;
                }

                default: {
                    return DefWindowProc(windowHandle, message, wordParam, longParam);
                }
            }

            break;
        }

        case WM_CLOSE: {
            DestroyWindow(windowHandle);
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        default: {
            return DefWindowProc(windowHandle, message, wordParam, longParam);
        }
    }

    return 0;
}
