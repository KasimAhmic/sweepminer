#define SDL_MAIN_USE_CALLBACKS 1

#include <filesystem>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "util.hpp"
#include "color.hpp"
#include "debug_menu.hpp"
#include "game.hpp"
#include "menu_bar.hpp"
#include "mouse.hpp"

#define UNUSED(x) (void)(x)

std::unique_ptr<Game> game;
std::unique_ptr<DebugMenu> debugMenu;
std::unique_ptr<MenuBar> menuBar;

constexpr Color WHITE(255, 255, 255, 255);
constexpr Color BLACK(0, 0, 0, 255);

struct AppContext {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, const int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC)) {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        return SDL_Fail();
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
        return SDL_Fail();
    }

    SDL_Log("base path: %s", SDL_GetBasePath());

    SDL_Window* window = SDL_CreateWindow("SweepMiner", 0, 0, SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!window) {
        SDL_Log("Couldn't create window: %s\n", SDL_GetError());
        return SDL_Fail();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Couldn't create renderer: %s\n", SDL_GetError());
        return SDL_Fail();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

    ImGui::StyleColorsLight();

    // ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(SDL_GetWindowDisplayScale(window));
    // style.FontScaleDpi = SDL_GetWindowDisplayScale(window);

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    const float scale = SDL_GetWindowDisplayScale(window);

    game = std::make_unique<Game>();
    menuBar = std::make_unique<MenuBar>(game.get());
    debugMenu = std::make_unique<DebugMenu>(game.get());

    game->newGame(Difficulty::BEGINNER, menuBar->getHeight());

    const SDL_FRect gameSize = game->getBoundingBox();
    SDL_SetRenderScale(renderer, scale, scale);

    SDL_SetWindowSize(window,
        static_cast<int32_t>(gameSize.w),
        static_cast<int32_t>(gameSize.h + menuBar->getHeight()));

    game->loadResources(renderer);

    // print some information about the window
    SDL_ShowWindow(window);
    {
        int32_t width, height, backBufferWidth, backBufferHeight;

        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &backBufferWidth, &backBufferHeight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Back buffer size: %ix%i", backBufferWidth, backBufferHeight);

        if (width != backBufferWidth){
            SDL_Log("This is a high DPI environment.");
        }
    }

    *appstate = new AppContext{
       .window = window,
       .renderer = renderer,
    };

    SDL_SetRenderVSync(renderer, 1);

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
    auto* app = static_cast<AppContext *>(appstate);

    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    switch (event->type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            Mouse::setButton(event->button.button);
            Mouse::setEvent(MouseEvent::BUTTON_DOWN);
            Mouse::setEventPosition(static_cast<int32_t>(event->button.x), static_cast<int32_t>(event->button.y));
            Mouse::setState(MouseState::DOWN);
            game->handleMouseEvent();
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Mouse::setButton(event->button.button);
            Mouse::setEvent(MouseEvent::BUTTON_UP);
            Mouse::setEventPosition(static_cast<int32_t>(event->button.x), static_cast<int32_t>(event->button.y));
            Mouse::setState(MouseState::UP);
            game->handleMouseEvent();
            break;
        }

        case SDL_EVENT_MOUSE_MOTION: {
            Mouse::setEvent(MouseEvent::MOVE);
            Mouse::setPosition(static_cast<int32_t>(event->motion.x), static_cast<int32_t>(event->motion.y));
            game->handleMouseEvent();
            break;
        }

        case SDL_EVENT_WINDOW_MOUSE_ENTER: {
            Mouse::setEvent(MouseEvent::ENTER);
            game->handleMouseEvent();
            break;
        }

        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
            Mouse::setEvent(MouseEvent::LEAVE);
            game->handleMouseEvent();
            break;
        }

        default: {
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto* app = static_cast<AppContext *>(appstate);

    SetRenderDrawColor(app->renderer, WHITE);
    SDL_RenderClear(app->renderer);

    int32_t windowWidth;
    int32_t windowHeight;

    SDL_GetWindowSize(app->window, &windowWidth, &windowHeight);

    game->draw(app->renderer);
    menuBar->draw(app->window, app->renderer);
    // debugMenu->draw(app->renderer);

    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

void SDL_AppQuit(void* appstate, const SDL_AppResult result) {
    UNUSED(result);

    if (const auto* app = static_cast<AppContext*>(appstate)) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);

        delete app;
    }

    if (game) {
        game.reset();
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    TTF_Quit();
    SDL_Log("Application quit successfully!");
    SDL_Quit();
}
