#pragma once

#include "Common.hpp"
#include "Vec.hpp"

TRE_NS_START

#define DECLARE_UNARY_VEC2_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	template<typename U, proc_type P> \
	FORCEINLINE Vec<2, TYPE, proctype>& Vec<2, TYPE, proctype>::operator opr (const Vec<2, U, P>& v){  \
		this->x opr static_cast<TYPE>(v.x);  \
		this->y opr static_cast<TYPE>(v.y);  \
		return *this;  \
	}  \
	template<typename TYPE>  \
	template<typename U>  \
		FORCEINLINE Vec<2, TYPE, proctype>& Vec<2, TYPE, proctype>::operator opr (U scalar){  \
		this->x opr static_cast<TYPE>(scalar);  \
		this->y opr static_cast<TYPE>(scalar);  \
		return *this;  \
	}  \

#define DECLARE_BINARY_VEC2_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	static FORCEINLINE Vec<2, TYPE, proctype> operator opr (const Vec<2, TYPE, proctype>& v, TYPE scalar) { \
		return Vec<2, TYPE, proctype>(v.x opr scalar, v.y opr scalar); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<2, TYPE, proctype> operator opr (TYPE scalar, Vec<2, TYPE, proctype> const& v) { \
		return Vec<2, TYPE, proctype>(scalar opr v.x, scalar opr v.y); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<2, TYPE, proctype> operator opr (Vec<2, TYPE, proctype> const& v1, Vec<2, TYPE, proctype> const& v2) {\
		return Vec<2, TYPE, proctype>(v1.x opr v2.x, v1.y opr v2.y); \
	} \

template<typename TYPE>
struct Vec<2, TYPE, normal>
{
	typedef TYPE value_type;
	typedef Vec<2, TYPE, normal> class_type;

#if defined(MATH_CONFIG_XYZW_ONLY)
	TYPE x, y;
#else
	union { 
		struct { TYPE x, y; }; 
		struct { TYPE r, g; }; 
		struct { TYPE s, t; }; 
		TYPE data[2];
	};
#endif
	FORCEINLINE Vec() : x(static_cast<TYPE>(0)), y(static_cast<TYPE>(0))
	{}

	FORCEINLINE explicit Vec(TYPE x, TYPE y) : x(static_cast<TYPE>(x)), y(static_cast<TYPE>(y))
	{}

	FORCEINLINE ~Vec() {}

	//Vec(const Vec& v) = default;
	//Vec(Vec&& v) = default;
	DEFINE_UNARY_OPERATOR(=, 2, normal);
	DEFINE_UNARY_OPERATOR(+=, 2, normal);
	DEFINE_UNARY_OPERATOR(-=, 2, normal);
	DEFINE_UNARY_OPERATOR(*=, 2, normal);
	DEFINE_UNARY_OPERATOR(/=, 2, normal);
	DEFINE_UNARY_OPERATOR(%=, 2, normal);
	DEFINE_UNARY_OPERATOR(&=, 2, normal);
	DEFINE_UNARY_OPERATOR(|=, 2, normal);
	DEFINE_UNARY_OPERATOR(^=, 2, normal);
	DEFINE_UNARY_OPERATOR(<<=, 2, normal);
	DEFINE_UNARY_OPERATOR(>>=, 2, normal);

	FORCEINLINE Vec<2, TYPE, normal>& operator++(void);	//prefix++: no parameter, returns a reference
	FORCEINLINE Vec<2, TYPE, normal> operator++(int);		//postfix++: dummy parameter, returns a value

	FORCEINLINE Vec<2, TYPE, normal>& operator--(void);	//prefix--: no parameter, returns a reference
	FORCEINLINE Vec<2, TYPE, normal> operator--(int);		//postfix--: dummy parameter, returns a value

	///**********OTHER OPERATIONS***********///

	FORCEINLINE const TYPE cross_product(const class_type&) const;

	FORCEINLINE const Vec<2, TYPE, normal> projection(const class_type& v) const;
	FORCEINLINE const TYPE scalar_projection(const class_type& v) const;

	FORCEINLINE TYPE dot_product(const class_type&) const;
	FORCEINLINE TYPE angle(const class_type&) const;

	FORCEINLINE TYPE lengthSqr() const;
	FORCEINLINE TYPE length() const;
	FORCEINLINE TYPE distance(const class_type&) const;
	FORCEINLINE const Vec<2, TYPE, normal> unit() const;
	
};

typedef Vec<2, float, normal> vec2;
typedef Vec<2, float, normal> Vec2f;

template<typename T>
FORCEINLINE Vec<2, T, normal> operator~(Vec<2, T, normal> const& v);

DEFINE_BINARY_OPERATOR(+, 2, normal)
DEFINE_BINARY_OPERATOR(-, 2, normal)
DEFINE_BINARY_OPERATOR(*, 2, normal)
DEFINE_BINARY_OPERATOR(/, 2, normal)
DEFINE_BINARY_OPERATOR(%, 2, normal)
DEFINE_BINARY_OPERATOR(&, 2, normal)
DEFINE_BINARY_OPERATOR(|, 2, normal)
DEFINE_BINARY_OPERATOR(^, 2, normal)
DEFINE_BINARY_OPERATOR(<<, 2, normal)
DEFINE_BINARY_OPERATOR(>>, 2, normal)

FORCEINLINE Vec<2, bool, normal> operator&&(const Vec<2, bool, normal>&, const Vec<2, bool, normal>&);
FORCEINLINE Vec<2, bool, normal> operator||(const Vec<2, bool, normal>&, const Vec<2, bool, normal>&);

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<2, TYPE, normal>&, const Vec<2, TYPE, normal>&);

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<2, TYPE, normal>&, const Vec<2, TYPE, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<2, float, normal>&, const Vec<2, float, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<2, float, normal>&, const Vec<2, float, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<2, double, normal>&, const Vec<2, double, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<2, double, normal>&, const Vec<2, double, normal>&);

TRE_NS_END

#include "Vec2.inl"

