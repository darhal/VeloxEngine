#pragma once

#include <math.h>
#include <float.h>
#include "Vec.hpp"

#define PI_180 0.01745329251

TRE_NS_START

template<uint8 N, typename TYPE, proc_type Q>
struct Vec;

template<typename TYPE>
struct Vec<1, TYPE, normal>;
template<typename TYPE>
struct Vec<2, TYPE, normal>;
template<typename TYPE>
struct Vec<3, TYPE, normal>;
template<typename TYPE>
struct Vec<4, TYPE, normal>;
template<typename TYPE, proc_type proc, uint8 R, uint8 C>
class Matrix;

template<typename T>
using Vec2 = TRE::Vec<2, T, normal>;
template<typename T>
using Vec3 = TRE::Vec<3, T, normal>;
template<typename T>
using Vec4 = TRE::Vec<4, T, normal>;

typedef Vec<1, float, normal> vec1;
typedef Vec<2, float, normal> vec2;
typedef Vec<3, float, normal> vec3;
typedef Vec<4, float, normal> vec4;
typedef Matrix<float, SIMD, 4, 4> mat4;

template<typename T>
static FORCEINLINE float rad(T deg)
{
	return float(PI_180 * (float)deg);
}

FORCEINLINE bool isEqual(float x, float y)
{
	return fabs(x - y) <= FLT_EPSILON;
}

template<typename T>
static T power(T base, int32 pow) {
	if (pow == 0) { return 1; }
	if (base == 0) { return 0; }
	T result = 1;
	if (pow > 0) {
		for (int i = 0; i < pow; i++) {
			result *= base;
		}
	}
	else {
		for (int i = 0; i < pow; i++) {
			result /= base;
		}
	}
	return result;
}

static int32 ParseUint64(char* str, uint32* x)
{
	*x = 0;
	if (str[0] >= '0' && str[0] <= '9') {
		uint8 data[21];
		int32 len = 0;
		while (str[len] >= '0' && str[len] <= '9' && len < 11) {
			data[len] = str[len] - '0';
			len++;
		}
		for (int32 i = 0; i < len; i++) {
			*x += data[i] * power(10u, len - i - 1);
		}
		return len;
	}
	return -1;
}

TRE_NS_END