#pragma once

#include <math.h>
#include <float.h>
#include "Vec.hpp"

#define PI_180 0.01745329251

template<typename T>
static FORCEINLINE float rad(T deg)
{
	return float(PI_180 * (float)deg);
}

FORCEINLINE bool isEqual(float x, float y)
{
	return fabs(x - y) <= FLT_EPSILON;
}