#pragma once

#include <concepts>

template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

class Scaler {
public:
    template <typename T> requires arithmetic<T>
    static float scaled(T value) {
        return static_cast<float>(value * userScale * deviceScale);
    }

    [[nodiscard]] static float getUserScale() { return Scaler::userScale; }
    static void setUserScale(const float value) { Scaler::userScale = value; }

    [[nodiscard]] static float getDeviceScale() { return Scaler::deviceScale; }
    static void setDeviceScale(const float value) { Scaler::deviceScale = value; }

    [[nodiscard]] static float getTotalScale();

private:
    static float userScale;
    static float deviceScale;
};
