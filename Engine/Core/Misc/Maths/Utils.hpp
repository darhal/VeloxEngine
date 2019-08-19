#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <type_traits>
#include <float.h>
#include <math.h>
#include <utility>

#define PI_180 0.01745329251

TRE_NS_START

template<typename T, typename std::enable_if<std::is_signed<T>::value == true, bool>::type = true>
static FORCEINLINE const T Absolute(T x)
{
	return (x < 0) ? -x : x;
}

template<typename T, typename std::enable_if<std::is_unsigned<T>::value == true, bool>::type = true>
static FORCEINLINE const T Absolute(T x)
{
	return x;
}

template<typename T>
static FORCEINLINE float rad(T deg)
{
	return float(PI_180 * (float)deg);
}

FORCEINLINE bool isEqual(float x, float y)
{
	return fabsf(x - y) <= FLT_EPSILON;
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

static float CompressFloat(float x, uint32 zerobits)
{
	union
	{
		float  f_rep;
		uint32 d_rep;
	} converter;
	converter.f_rep = x;
  	// mask is e.g. 0xffffffffffff0000 for zerobits==16
 	uint32 mask = (uint32(-1) << zerobits);  
  	uint32 floatbits = converter.d_rep;// (*((uint32*)(&x)));
  	floatbits &= mask;
	converter.d_rep = floatbits;
  	// x =  *((float*) (&floatbits));
  	return converter.f_rep;
}

TRE_NS_END