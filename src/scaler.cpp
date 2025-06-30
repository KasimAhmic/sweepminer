#include "scaler.hpp"

int32_t Scaler::userScale = 1;
int32_t Scaler::deviceScale = 1;

int32_t Scaler::getTotalScale() {
    return Scaler::userScale * Scaler::deviceScale;
}
