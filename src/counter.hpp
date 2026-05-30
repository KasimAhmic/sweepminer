#pragma once

#include <array>

#include "box.hpp"

class Counter : public Box {
public:
    static constexpr float BORDER_WIDTH = 1.0f;
    static constexpr float SEGMENT_WIDTH = 13.0f;
    static constexpr float SEGMENT_HEIGHT = 23.0f;

    explicit Counter(Context* context, const SDL_FRect& rect);
    ~Counter() override;

    void render() override;

    void increment() { this->value++; }
    void decrement() { this->value--; }

    [[nodiscard]] uint16_t getValue() const { return this->value; }

private:
    uint16_t value{0};

    [[nodiscard]] std::array<uint8_t, 3> getDigits() const;
};
