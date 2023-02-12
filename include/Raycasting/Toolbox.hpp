#pragma once

#include "SDL.h"

namespace Math
{
    const double DEGREE_TO_RADIAN = M_PI / 180.0;
    template <typename T>
    inline T limitToInterval(T value, T min, T max) { if (value < min) return min; if (value > max) return max; return value; }
}