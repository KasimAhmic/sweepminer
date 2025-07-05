#include "scaler.hpp"

float Scaler::userScale = 1.0f;
float Scaler::deviceScale = 1.0f;

float Scaler::getTotalScale() {
    return Scaler::userScale * Scaler::deviceScale;
}
