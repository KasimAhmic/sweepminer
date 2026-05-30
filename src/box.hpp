#pragma once

#include "color.hpp"
#include "ui_component.hpp"

class Box : public UiComponent {
public:
    explicit Box(Context* context,
                 const SDL_FRect& rect,
                 float borderWidth,
                 const Color& borderHighlightColor,
                 const Color& borderShadowColor,
                 const Color& backgroundColor);
    ~Box() override;

    void render() override;

    [[nodiscard]] float getBorderWidth() const { return this->borderWidth; }
    void setBorderWidth(const float width) { this->borderWidth = width; }

    [[nodiscard]] const Color& getBorderPrimaryColor() const { return this->borderPrimaryColor; }
    void setBorderPrimaryColor(const Color& color) { this->borderPrimaryColor = color; }

    [[nodiscard]] const Color& getBorderSecondaryColor() const { return this->borderSecondaryColor; }
    void setBorderSecondaryColor(const Color& color) { this->borderSecondaryColor = color; }

    [[nodiscard]] const Color& getBackgroundColor() const { return this->backgroundColor; }
    void setBackgroundColor(const Color& color) { this->backgroundColor = color; }

private:
    float borderWidth;
    Color borderPrimaryColor;
    Color borderSecondaryColor;
    Color backgroundColor;
};
