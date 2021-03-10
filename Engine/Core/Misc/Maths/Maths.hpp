#pragma once

#include <Engine/Core/Misc/Defines/Common.hpp>
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

	template<typename T>
	FORCEINLINE static const T Floor(T x) { return floor(x); }

	template<typename T>
	FORCEINLINE static const T Round(T x) { return round(x); }

	template<typename T>
	FORCEINLINE static const T Ceil(T x) { return ceil(x); }

	template<typename T>
	FORCEINLINE static const T DivCeil(T x, T y) { return 1 + ((x - 1) / y); }

	template<typename T>
	FORCEINLINE static const T DivFloor(T x, T y) { return x / y; }

	template<typename T>
	FORCEINLINE static const T Log(T x) { return log(x); }

	template<typename T>
	FORCEINLINE static const T Log(T x, uint32 base) { return log(x) / log(base); }

	template<typename T>
	FORCEINLINE static const T Max(T x, T y) { return MAX(x, y); }

	template<typename T>
	FORCEINLINE static const T Min(T x, T y) { return MIN(x, y); }

	template<typename Last>
	FORCEINLINE static const Last MMax(Last x) { return x; }

	template<typename Last>
	FORCEINLINE static const Last MMin(Last x) { return x; }

	template<typename F, typename... T>
	FORCEINLINE static const F MMax(F x, T... other) { return Max(x, Math::MMax<T...>(other...)); }

	template<typename F, typename... T>
	FORCEINLINE static const F MMin(F x, T... other) { return Min(x, Math::MMin<T...>(other...)); }

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
			uint64  d_rep;
		} converter;

		converter.f_rep = x;
		uint64 mask = (uint64(-1) << zerobits); // mask is e.g. 0xffffffffffff0000 for zerobits==16
		uint64 floatbits = converter.d_rep;
		floatbits &= mask;
		converter.d_rep = floatbits;
		return converter.f_rep;
	}

	FORCEINLINE static usize HashFloat(float x)
	{
		union
		{
			float  f_rep;
			uint32 d_rep;
		} converter;

		converter.f_rep = x;
		return (usize) converter.d_rep;
	}

	FORCEINLINE static usize HashDouble(double x)
	{
		union
		{
			double  f_rep;
			uint64  d_rep;
		} converter;

		converter.f_rep = x;
		return (usize) converter.d_rep;
	}

	template<typename T>
	FORCEINLINE static uint32 BinarySearch(const T* arr, uint32 start, uint32 end, const T& search)
	{
		while (start <= end) {
			uint32 m = start + (end - start) / 2;

			if (arr[m] == search) // Check if x is present at mid 
				return m;
			else if (arr[m] < search) // If x greater, ignore left half 
				start = m + 1;
			else // If x is smaller, ignore right half 
				end = m - 1;
		}

		// element was not present 
		return -1;
	}


    FORCEINLINE static uint32 NextPow2(uint32 v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    FORCEINLINE static uint64 NextPow2(uint64 v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        v++;
        return v;
    }


#ifdef _MSC_VER
static inline int __builtin_ctz(unsigned x) {
    unsigned long ret;
    _BitScanForward(&ret, x);
    return (int)ret;
}

static inline int __builtin_ctzll(unsigned long long x) {
    unsigned long ret;
    _BitScanForward64(&ret, x);
    return (int)ret;
}

static inline int __builtin_ctzl(unsigned long x) {
    return sizeof(x) == 8 ? __builtin_ctzll(x) : __builtin_ctz((uint32_t)x);
}

static inline int __builtin_clz(unsigned x) {
    //unsigned long ret;
    //_BitScanReverse(&ret, x);
    //return (int)(31 ^ ret);
    return (int)__lzcnt(x);
}

static inline int __builtin_clzll(unsigned long long x) {
    //unsigned long ret;
    //_BitScanReverse64(&ret, x);
    //return (int)(63 ^ ret);
    return (int)__lzcnt64(x);
}

static inline int __builtin_clzl(unsigned long x) {
    return sizeof(x) == 8 ? __builtin_clzll(x) : __builtin_clz((uint32_t)x);
}

#ifdef __cplusplus
static inline int __builtin_ctzl(unsigned long long x) {
    return __builtin_ctzll(x);
}

static inline int __builtin_clzl(unsigned long long x) {
    return __builtin_clzll(x);
}
#endif
#endif

    FORCEINLINE static uint32 Log2OfPow2(uint32 value) {
    #if defined(COMPILER_CLANG) || defined (COMPILER_GCC) || defined(COMPILER_MSVC)
        return sizeof(uint32_t) * CHAR_BIT - __builtin_clz(value) - 1;
    #else
        return log2(value);
    #endif
    }

};

TRE_NS_END
