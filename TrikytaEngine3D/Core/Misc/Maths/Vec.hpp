#pragma once

#include <Core/Misc/Defines/Common.hpp>

enum proc_type {
	normal,
	SIMD,
};

template<uint8 N, typename TYPE, proc_type Q>
struct Vec;

#define DEFINE_UNARY_OPERATOR(opr, i, proctype) \
	template<typename U, proc_type P> \
	FORCEINLINE Vec<i, TYPE, proctype>& operator opr (const Vec<i, U, P>&); \
	template<typename U> \
	FORCEINLINE Vec<i, TYPE, proctype>& operator opr (U); \

#define DEFINE_BINARY_OPERATOR(opr, i, proctype) \
	template<typename TYPE> \
	static FORCEINLINE Vec<i, TYPE, proctype> operator opr (const Vec<i, TYPE, proctype>&, TYPE); \
	template<typename TYPE> \
	static FORCEINLINE Vec<i, TYPE, proctype> operator opr (TYPE, const Vec<i, TYPE, proctype>&); \
	template<typename TYPE> \
	static FORCEINLINE Vec<i, TYPE, proctype> operator opr (const Vec<i, TYPE, proctype>&, const Vec<i, TYPE, proctype>&); \

template<typename TYPE>
struct Vec<2, TYPE, normal>;
template<typename TYPE>
struct Vec<3, TYPE, normal>;