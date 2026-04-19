#define SDL_MAIN_USE_CALLBACKS 1

#include <exception>

#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#ifdef SWEEPMINER_PLATFORM_OTHER
#include <imgui_impl_sdl3.h>
#endif

#include "color.hpp"
#include "events.hpp"
#include "game.hpp"
#include "util.hpp"
#include "profiler.hpp"
#include "menu_bar.hpp"

#define UNUSED(x) (void)(x)

struct AppState {
    uint64_t lastCounter{};
    double deltaTime{};
    std::unique_ptr<Game> game{};
    std::unique_ptr<IMenuBar> menuBar{};
    std::unique_ptr<Profiler> profiler{};
};

#ifdef SWEEPMINER_PLATFORM_WINDOWS
#include <Windows.h>

bool HandleWindowsMessage(void* userdata, MSG* msg) {
    const auto app = static_cast<AppState*>(userdata);

    if (msg->message != WM_COMMAND) {
        return true;
    }

    app->menuBar->handleMenuClick(LOWORD(msg->wParam));

    return false;
}
#endif

void SDL_QuitAll() {
    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
}

void SDL_DestroyAll(SDL_Window* window = nullptr,
                    SDL_Renderer* renderer = nullptr,
                    TTF_TextEngine* textEngine = nullptr,
                    MIX_Mixer* mixer = nullptr) {
    MIX_DestroyMixer(mixer);
    TTF_DestroyRendererTextEngine(textEngine);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

SDL_AppResult SDL_AppInit(void** appstate, const int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    *appstate = nullptr;

    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_QuitAll();

        return SDL_Fail("Couldn't initialize SDL");
    }

    if (!TTF_Init()) {
        SDL_QuitAll();

        return SDL_Fail("Couldn't initialize SDL_ttf");
    }

    if (!MIX_Init()) {
        SDL_QuitAll();

        return SDL_Fail("Couldn't initialize SDL_mixer");
    }

    SDL_Window* window = SDL_CreateWindow("SweepMiner", 200, 200, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        SDL_QuitAll();

        return SDL_Fail("Couldn't create window");
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_DestroyAll(window);
        SDL_QuitAll();

        return SDL_Fail("Couldn't create renderer");
    }

    TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
    if (!textEngine) {
        SDL_DestroyAll(window, renderer);
        SDL_QuitAll();

        return SDL_Fail("Couldn't create text engine");
    }

    MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        SDL_DestroyAll(window, renderer, textEngine);
        SDL_QuitAll();

        return SDL_Fail("Couldn't create mixer");
    }

    MIX_Track* track = MIX_CreateTrack(mixer);
    if (!track) {
        SDL_DestroyAll(window, renderer, textEngine, mixer);
        SDL_QuitAll();

        return SDL_Fail("Couldn't create track");
    }

    SDL_Log("Renderer: %s", SDL_GetRendererName(renderer));

    SDL_SetRenderVSync(renderer, 1);
    SDL_ShowWindow(window);

    std::unique_ptr<IMenuBar> menuBar = CreateMenuBar(window);

#ifdef SWEEPMINER_PLATFORM_MACOS
    menuBar->addMenu(ID_APP_MENU, "SweepMiner");
    menuBar->addItem(ID_APP_ABOUT, ID_APP_MENU, "About");
    menuBar->addItem(ID_APP_QUIT, ID_APP_MENU, "Quit");
#endif
    menuBar->addMenu(ID_GAME_MENU, "Game");
    menuBar->addItem(ID_GAME_NEW, ID_GAME_MENU, "New Game");
    menuBar->addSeparator(ID_GAME_MENU);
    menuBar->addItem(ID_GAME_BEGINNER, ID_GAME_MENU, "Beginner");
    menuBar->addItem(ID_GAME_INTERMEDIATE, ID_GAME_MENU, "Intermediate");
    menuBar->addItem(ID_GAME_EXPERT, ID_GAME_MENU, "Expert");
    menuBar->addItem(ID_GAME_CUSTOM, ID_GAME_MENU, "Custom...");
    menuBar->addSeparator(ID_GAME_MENU);
    menuBar->addItem(ID_GAME_MARKS, ID_GAME_MENU, "Marks (?)");
    menuBar->addItem(ID_GAME_COLOR, ID_GAME_MENU, "Color");
    menuBar->addItem(ID_GAME_SOUND, ID_GAME_MENU, "Sound");
    menuBar->addSeparator(ID_GAME_MENU);
    menuBar->addItem(ID_GAME_HIGHSCORES, ID_GAME_MENU, "Best Times...");
#ifndef SWEEPMINER_PLATFORM_MACOS
    menuBar->addSeparator(ID_GAME_MENU);
    menuBar->addItem(ID_GAME_EXIT, ID_GAME_MENU, "Exit");
#endif

    menuBar->addMenu(ID_VIEW_MENU, "View");
    menuBar->addSubMenu(ID_VIEW_ZOOM_MENU, ID_VIEW_MENU, "Zoom");
    menuBar->addItem(ID_VIEW_ZOOM_IN, ID_VIEW_ZOOM_MENU, "Zoom In");
    menuBar->addItem(ID_VIEW_ZOOM_OUT, ID_VIEW_ZOOM_MENU, "Zoom Out");
    menuBar->addItem(ID_VIEW_ZOOM_RESET, ID_VIEW_ZOOM_MENU, "Reset Zoom");

    menuBar->addMenu(ID_HELP_MENU, "Help");
    menuBar->addItem(ID_HELP_GITHUB, ID_HELP_MENU, "GitHub");
    menuBar->addItem(ID_HELP_REPORT_ISSUE, ID_HELP_MENU, "Report an Issue");

    Events::init();

    try {
        auto game = std::make_unique<Game>(window, renderer, textEngine, mixer, track, menuBar->getHeight());

        game->init();

#if SWEEPMINER_ENABLE_PROFILER
        auto* profiler = new Profiler(renderer, textEngine);
#else
        constexpr auto profiler = nullptr;
#endif

        *appstate = new AppState{
            .lastCounter = SDL_GetPerformanceCounter(),
            .deltaTime = 0.0,
            .game = std::move(game),
            .menuBar = std::move(menuBar),
            .profiler = profiler,
        };

#ifdef SWEEPMINER_PLATFORM_WINDOWS
        SDL_SetWindowsMessageHook(HandleWindowsMessage, *appstate);
#endif
    } catch (const std::exception& e) {
        MIX_Quit();
        TTF_Quit();
        SDL_Quit();

        return SDL_Fail(e.what());
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Received SDL_Event: %s", SDL_GetEventName(event->type));

    const auto app = static_cast<AppState*>(appstate);

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

#ifdef SWEEPMINER_PLATFORM_OTHER
    if (ImGui::GetIO().WantCaptureMouse && ImGui_ImplSDL3_ProcessEvent(event)) {
        return SDL_APP_CONTINUE;
    }
#endif

    if (event->type == Events::MENU_CLICK) {
        const int32_t code = event->user.code;

        if (code == ID_GAME_EXIT || code == ID_APP_QUIT) {
            return SDL_APP_SUCCESS;
        }

        if (code == ID_GAME_NEW) {
            app->game->start();
        } else if (code == ID_GAME_BEGINNER) {
            app->game->setDifficulty(Game::Difficulty::BEGINNER);
            app->game->start();
        } else if (code == ID_GAME_INTERMEDIATE) {
            app->game->setDifficulty(Game::Difficulty::INTERMEDIATE);
            app->game->start();
        } else if (code == ID_GAME_EXPERT) {
            app->game->setDifficulty(Game::Difficulty::EXPERT);
            app->game->start();
        } else if (code == ID_HELP_GITHUB) {
            SDL_OpenURL("https://github.com/KasimAhmic/SweepMiner");
        } else if (code == ID_HELP_REPORT_ISSUE) {
            SDL_OpenURL("https://github.com/KasimAhmic/SweepMiner/issues");
        }

        return SDL_APP_CONTINUE;
    }

    app->game->handleEvent(*event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    ProfileCall("App Iterate", {
        const auto app = static_cast<AppState*>(appstate);
        const uint64_t currentCounter = SDL_GetPerformanceCounter();

        app->deltaTime =
            static_cast<double>(currentCounter - app->lastCounter) /
            static_cast<double>(SDL_GetPerformanceFrequency());

        app->lastCounter = currentCounter;

        SDL_SetRenderDrawColor(app->game->getContext().getRenderer(), SpreadColorInt(BLACK));
        SDL_RenderClear(app->game->getContext().getRenderer());

        app->game->render(app->deltaTime);
        app->menuBar->render();

        SDL_RenderPresent(app->game->getContext().getRenderer());
    })

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, const SDL_AppResult result) {
    SDL_Log("Application quitting with result code %d", result);

    const auto app = static_cast<AppState*>(appstate);

    delete app;

    SDL_QuitAll();
}
