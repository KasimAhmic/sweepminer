#pragma once

#include "color.hpp"
#include "component.hpp"

class Box : public Component {
public:
    explicit Box(const AppContext &context, const SDL_FRect &rect)
        : Component(context, rect) {}
    ~Box() override = default;

    void render() override;

    [[nodiscard]] Color getBackgroundColor() const { return this->backgroundColor; }
    Box& setBackgroundColor(const Color& color) {
        this->backgroundColor = color;
        return *this;
    }

    [[nodiscard]] Color getBorderHighlightColor() const { return this->borderHighlightColor; }
    Box& setBorderHighlightColor(const Color& color) {
        this->borderHighlightColor = color;
        return *this;
    }

    [[nodiscard]] Color getBorderShadowColor() const { return this->borderShadowColor; }
    Box& setBorderShadowColor(const Color& color) {
        this->borderShadowColor = color;
        return *this;
    }

    [[nodiscard]] uint8_t getBorderWidth() const { return this->borderWidth; }
    Box& setBorderWidth(const uint8_t& width) {
        this->borderWidth = width;
        return *this;
    }

private:
    Color backgroundColor = { 0, 0, 0, 0 };
    Color borderHighlightColor = { 0, 0, 0, 0 };
    Color borderShadowColor = { 0, 0, 0, 0 };
    uint8_t borderWidth = 0;
};
