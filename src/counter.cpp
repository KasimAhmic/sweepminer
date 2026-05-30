#include "counter.hpp"

#include "textures.hpp"

Counter::Counter(Context *context, const SDL_FRect &rect)
    : Box(context, rect, BORDER_WIDTH, DARK_GREY, WHITE, GREY) {}

Counter::~Counter() = default;

void Counter::render() {
    Box::render();

    const auto [x, y, w, h] = this->getRect();

    const std::array<uint8_t, 3> digits = this->getDigits();

    for (size_t i = 0; i < digits.size(); i++) {
        SDL_FRect dest{
            .x = x + (i * SEGMENT_WIDTH + BORDER_WIDTH) * this->getContext().getDisplayScale(),
            .y = y + BORDER_WIDTH * this->getContext().getDisplayScale(),
            .w = SEGMENT_WIDTH * this->getContext().getDisplayScale(),
            .h = SEGMENT_HEIGHT * this->getContext().getDisplayScale(),
        };

        SDL_RenderTexture(
            this->getContext().getRenderer(),
            this->getContext().getResourceManager().getTexture(ResourceManager::Texture::NUMBERS),
            TextureOffset::getNumberTextureOffset(digits.at(i)),
            &dest);
    }
}

std::array<uint8_t, 3> Counter::getDigits() const {
    if (const uint16_t val = this->getValue(); val >= 999) {
        return std::array<uint8_t, 3>{9, 9, 9};
    }

    return {
        static_cast<uint8_t>(this->getValue() / 100 % 10), // Pull out the first digit
        static_cast<uint8_t>(this->getValue() /  10 % 10), // Pull out the second digit
        static_cast<uint8_t>(this->getValue() /   1 % 10)  // Pull out the third digit
    };
}
