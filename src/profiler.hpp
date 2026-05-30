#pragma once

#include <map>

#include <SDL3/SDL.h>

#include "ui_component.hpp"

#if SWEEPMINER_ENABLE_PROFILER
#define ProfileCall(label, func) Profiler::getInstance().measure(label, [&] { func; });
#else
#define ProfileCall(label, func) func;
#endif

class Profiler {
public:
    static constexpr double UPDATE_INTERVAL = 0.5;
    static constexpr float FONT_SIZE = 8.0;
    static constexpr float PADDING = 5.0;

    explicit Profiler(SDL_Renderer* renderer, TTF_TextEngine* textEngine);
    ~Profiler();

    static Profiler& getInstance();

    template <typename Func>
    void measure(const char* label, Func&& func) {
        const uint64_t start = SDL_GetPerformanceCounter();
        func();
        const uint64_t end = SDL_GetPerformanceCounter();

        const double us = static_cast<double>(end - start) * 1'000'000.0 / freq;
        auto&[lastUs, avgUs, maxUs, calls] = this->stats[label];
        lastUs = us;
        calls++;
        maxUs = us > maxUs ? us : maxUs;
        avgUs = calls == 1 ? us : avgUs * 0.9 + us * 0.1;
    }

    void render(double deltaTime);

private:
    struct Stat {
        double lastUs{0.0};
        double avgUs{0.0};
        double maxUs{0.0};
        uint64_t calls{0};
    };

    static Profiler* instance;

    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    SDL_FRect rect;

    double freq;
    double accumulator;
    uint64_t frameCount;
    double fps;
    std::map<const char *, Stat> stats;

    SDL_Texture* background;
    TTF_Font* font;
    TTF_Text* text;
};
