#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "Maths.hpp"

TRE_NS_START

enum proc_type {
	normal,
	SIMD,
};

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

TRE_NS_END