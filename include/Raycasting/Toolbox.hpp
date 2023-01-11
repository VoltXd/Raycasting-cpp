#pragma once

#include "SDL.h"

namespace Math
{
    const double DEGREE_TO_RADIAN = M_PI / 180.0;
    inline double limitToInterval(double value, double min, double max) { if (value < min) return min; if (value > max) return max; return value; }
}