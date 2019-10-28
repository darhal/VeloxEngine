#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <math.h>
#include <ctgmath>
#include <type_traits>
#include <float.h>
#include <utility>

TRE_NS_START

struct Math
{
	CONSTEXPR static double PI				= 3.141592653589793;
	CONSTEXPR static double PI_PER_180		= PI / 180.0;
	CONSTEXPR static double D180_PER_PI		= 180.0 / PI;

	/*
	* Trigonometric functions : 
	*/
	FORCEINLINE static float Cos(float x) { return (float) cos(x); }

	FORCEINLINE static double Cos(double x) { return cos(x); }

	FORCEINLINE static float Sin(float x) { return (float) sin(x); }

	FORCEINLINE static double Sin(double x) { return sin(x); }

	FORCEINLINE static float Tan(float x) { return tan(x); }

	FORCEINLINE static double Tan(double x) { return tan(x); }

	FORCEINLINE static float Atan2(float x, float y) { return atan2(x, y); }

	FORCEINLINE static double Atan2(double x, double y) { return atan2(x, y); }

	FORCEINLINE static float ToRad(float deg) { return (float)PI_PER_180 * deg; }

	FORCEINLINE static double ToRad(double deg) { return PI_PER_180 * deg; }

	FORCEINLINE static float ToDeg(float rad) { return (float)D180_PER_PI * rad; }

	FORCEINLINE static double ToDeg(double rad) { return D180_PER_PI * rad; }

	/*
	*Arithmetic operations : 
	*/
	template<typename T, typename std::enable_if<std::is_signed<T>::value == true, bool>::type = true>
	FORCEINLINE static const T Abs(T x) { return (x < 0) ? -x : x; }

	template<typename T, typename std::enable_if<std::is_unsigned<T>::value == true, bool>::type = true>
	FORCEINLINE static const T Abs(T x) { return x; }

	/*
	*Floating point operations : 
	*/
	FORCEINLINE static bool FloatEqual(float x, float y) { return fabsf((x - y)) < FLT_EPSILON; }

	FORCEINLINE static bool FloatEqual(double x, double y) { return fabs((x - y)) < FLT_EPSILON; }

	template<typename T>
	FORCEINLINE static T Sqr(T x) { return x * x; }

	FORCEINLINE static float Pow(float x, float y) { return powf(x, y); }

	FORCEINLINE static double Pow(double x, double y) { return pow(x, y); }

	FORCEINLINE static float Sqrt(float x) { return sqrtf(x); }

	FORCEINLINE static double Sqrt(double x) { return sqrt(x); }

	FORCEINLINE static float CompressFloat(float x, uint32 zerobits)
	{
		union
		{
			float  f_rep;
			uint32 d_rep;
		} converter;

		converter.f_rep = x;
		uint32 mask = (uint32(-1) << zerobits); // mask is e.g. 0xffffffffffff0000 for zerobits==16
		uint32 floatbits = converter.d_rep;
		floatbits &= mask;
		converter.d_rep = floatbits;
		return converter.f_rep;
	}

	FORCEINLINE static double CompressDouble(double x, uint32 zerobits)
	{
		union
		{
			double  f_rep;
			uint64 d_rep;
		} converter;

		converter.f_rep = x;
		uint64 mask = (uint64(-1) << zerobits); // mask is e.g. 0xffffffffffff0000 for zerobits==16
		uint64 floatbits = converter.d_rep;
		floatbits &= mask;
		converter.d_rep = floatbits;
		return converter.f_rep;
	}
};

TRE_NS_END