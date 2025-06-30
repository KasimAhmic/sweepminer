#pragma once

#include <concepts>
#include <cstdint>

template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

class Scaler {
public:
    template <typename T> requires arithmetic<T>
    static float scaled(T value) {
        return static_cast<float>(value * userScale * deviceScale);
    }

    [[nodiscard]] static int32_t getUserScale() { return Scaler::userScale; }
    static void setUserScale(const int32_t value) { Scaler::userScale = value; }

    [[nodiscard]] static int32_t getDeviceScale() { return Scaler::deviceScale; }
    static void setDeviceScale(const int32_t value) { Scaler::deviceScale = value; }

    [[nodiscard]] static int32_t getTotalScale() { return Scaler::userScale * Scaler::deviceScale; }

private:
    static int32_t userScale;
    static int32_t deviceScale;
};
