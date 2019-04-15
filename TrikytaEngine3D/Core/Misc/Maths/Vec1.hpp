#pragma once

#include "Common.hpp"
#include "Vec.hpp"


#define DECLARE_UNARY_VEC1_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	template<typename U, proc_type P> \
	FORCEINLINE Vec<1, TYPE, proctype>& Vec<1, TYPE, proctype>::operator opr (const Vec<1, U, P>& v){  \
		this->x  opr  static_cast<TYPE>(v.x);  \
		return *this;  \
	}  \
	template<typename TYPE>  \
	template<typename U>  \
		FORCEINLINE Vec<1, TYPE, proctype>& Vec<1, TYPE, proctype>::operator opr (U scalar){  \
		this->x  opr  static_cast<TYPE>(scalar);  \
		return *this;  \
	}  \

#define DECLARE_BINARY_VEC1_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	static FORCEINLINE Vec<1, TYPE, proctype> operator  opr  (const Vec<1, TYPE, proctype>& v, TYPE scalar) { \
		return Vec<1, TYPE, proctype>(v.x  opr  scalar); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<1, TYPE, proctype> operator  opr  (TYPE scalar, Vec<1, TYPE, proctype> const& v) { \
		return Vec<1, TYPE, proctype>(scalar  opr  v.x); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<1, TYPE, proctype> operator  opr  (Vec<1, TYPE, proctype> const& v1, Vec<1, TYPE, proctype> const& v2) {\
		return Vec<1, TYPE, proctype>(v1.x  opr  v2.x); \
	} \

TRE_NS_START

template<typename TYPE>
struct Vec<1, TYPE, normal>
{
	typedef TYPE value_type;

#if defined(MATH_CONFIG_XYZW_ONLY)
	TYPE x;
#else
	union { TYPE x, r, s; TYPE data[1];};
#endif
	FORCEINLINE Vec() : x((TYPE)0)
	{}

	FORCEINLINE explicit Vec(TYPE x) : x(static_cast<TYPE>(x))
	{}

	FORCEINLINE ~Vec() {}

	//Vec(const Vec& v) = default;
	//Vec(Vec&& v) = default;
	DEFINE_UNARY_OPERATOR(=, 1, normal)
	DEFINE_UNARY_OPERATOR(+=, 1, normal)
	DEFINE_UNARY_OPERATOR(-=, 1, normal)
	DEFINE_UNARY_OPERATOR(*=, 1, normal)
	DEFINE_UNARY_OPERATOR(/=, 1, normal)
	DEFINE_UNARY_OPERATOR(%=, 1, normal)
	DEFINE_UNARY_OPERATOR(&=, 1, normal)
	DEFINE_UNARY_OPERATOR(|=, 1, normal)
	DEFINE_UNARY_OPERATOR(^=, 1, normal)
	DEFINE_UNARY_OPERATOR(<<=, 1, normal)
	DEFINE_UNARY_OPERATOR(>>=, 1, normal)

	FORCEINLINE Vec<1, TYPE, normal>& operator++(void);	//prefix++: no parameter, returns a reference
	FORCEINLINE Vec<1, TYPE, normal> operator++(int);		//postfix++: dummy parameter, returns a value

	FORCEINLINE Vec<1, TYPE, normal>& operator--(void);	//prefix--: no parameter, returns a reference
	FORCEINLINE Vec<1, TYPE, normal> operator--(int);		//postfix--: dummy parameter, returns a value	
};

template<typename T>
FORCEINLINE Vec<1, T, normal> operator~(Vec<1, T, normal> const& v);

DEFINE_BINARY_OPERATOR(+, 1, normal)
DEFINE_BINARY_OPERATOR(-, 1, normal)
DEFINE_BINARY_OPERATOR(*, 1, normal)
DEFINE_BINARY_OPERATOR(/, 1, normal)
DEFINE_BINARY_OPERATOR(%, 1, normal)
DEFINE_BINARY_OPERATOR(&, 1, normal)
DEFINE_BINARY_OPERATOR(|, 1, normal)
DEFINE_BINARY_OPERATOR(^, 1, normal)
DEFINE_BINARY_OPERATOR(<<, 1, normal)
DEFINE_BINARY_OPERATOR(>>, 1, normal)

FORCEINLINE Vec<1, bool, normal> operator&&(const Vec<1, bool, normal>&, const Vec<1, bool, normal>&);
FORCEINLINE Vec<1, bool, normal> operator||(const Vec<1, bool, normal>&, const Vec<1, bool, normal>&);

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<1, TYPE, normal>&, const Vec<1, TYPE, normal>&);

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<1, TYPE, normal>&, const Vec<1, TYPE, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<1, float, normal>&, const Vec<1, float, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<1, float, normal>&, const Vec<1, float, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<1, double, normal>&, const Vec<1, double, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<1, double, normal>&, const Vec<1, double, normal>&);

TRE_NS_END

#include "Vec1.inl"
