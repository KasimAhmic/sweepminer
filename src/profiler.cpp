#include "profiler.hpp"

#include <format>
#include <ranges>

#include "resource_manager.hpp"

Profiler* Profiler::instance = nullptr;

Profiler::Profiler(SDL_Renderer* renderer, TTF_TextEngine* textEngine)
    : renderer(renderer),
      textEngine(textEngine),
      rect(SDL_FRect{}),
      freq(static_cast<double>(SDL_GetPerformanceFrequency())),
      accumulator(0.0),
      frameCount(0),
      fps(0.0)
{
    SDL_Surface* surface = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_RGBA32);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(surface->format);
    SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGBA(details, nullptr, 0, 0, 0, 200));

    this->background = SDL_CreateTextureFromSurface(this->renderer, surface);
    SDL_DestroySurface(surface);

    SDL_SetTextureBlendMode(this->background, SDL_BLENDMODE_BLEND);

    this->font = TTF_OpenFont("assets/fonts/SourceCodePro-Medium.ttf", FONT_SIZE * SDL_GetWindowDisplayScale(SDL_GetRenderWindow(this->renderer)));

    this->text = TTF_CreateText(
        this->textEngine,
        this->font,
        "",
        0);
    TTF_SetTextWrapWidth(this->text, 0);

    Profiler::instance = this;
}

Profiler::~Profiler() {
    TTF_CloseFont(this->font);
}

Profiler& Profiler::getInstance() {
    if (instance == nullptr) {
        throw std::runtime_error("Profiler has not been initialized");
    }

    return *instance;
}

void Profiler::render(const double deltaTime) {
    this->frameCount++;
    this->accumulator += deltaTime;

    if (this->accumulator >= UPDATE_INTERVAL) {
        this->fps = static_cast<double>(this->frameCount) / this->accumulator;

        uint16_t maxLen = 0;

        for (const auto &label: this->stats | std::views::keys) {
            if (const size_t len = strlen(label); len > maxLen) {
                maxLen = len;
            }
        }

        std::string newText = std::format("{:<{}}: {:.2f}\n", "FPS", maxLen, this->fps);

        for (const auto&[label, stat]: this->stats) {
            newText.append(std::format(
                "{:<{}}: {:>8.2f} (avg: {:>8.2f}, max: {:>8.2f})\n",
                label,
                maxLen,
                stat.lastUs,
                stat.avgUs,
                stat.maxUs
            ));
        }

        TTF_SetTextString(this->text, newText.c_str(), 0);

        this->frameCount = 0;
        this->accumulator = 0.0;
    }

    int w{};
    int h{};

    TTF_GetStringSizeWrapped(
        this->font,
        this->text->text, 0, 0, &w, &h);

    const SDL_FRect bgRect{
        .x = 0,
        .y = 0,
        .w = static_cast<float>(w) + PADDING * 2,
        .h = static_cast<float>(h) + PADDING * 2
    };

    SDL_RenderTexture(this->renderer, this->background, nullptr, &bgRect);
    TTF_DrawRendererText(this->text, this->rect.x + PADDING, this->rect.y + PADDING);
}
