#pragma once

#include "Common.hpp"
#include "Vec.hpp"
#include "Vec2.hpp"

TRE_NS_START

#define DECLARE_UNARY_VEC3_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	template<typename U, proc_type P> \
	FORCEINLINE Vec<3, TYPE, proctype>& Vec<3, TYPE, proctype>::operator opr (const Vec<3, U, P>& v){  \
		this->x opr static_cast<TYPE>(v.x);  \
		this->y opr static_cast<TYPE>(v.y);  \
		this->z opr static_cast<TYPE>(v.z);  \
		return *this;  \
	}  \
	template<typename TYPE>  \
	template<typename U>  \
		FORCEINLINE Vec<3, TYPE, proctype>& Vec<3, TYPE, proctype>::operator opr (U scalar){  \
		this->x opr static_cast<TYPE>(scalar);  \
		this->y opr static_cast<TYPE>(scalar);  \
		this->z opr static_cast<TYPE>(scalar);  \
		return *this;  \
	}  \

#define DECLARE_BINARY_VEC3_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	static FORCEINLINE Vec<3, TYPE, proctype> operator opr (const Vec<3, TYPE, proctype>& v, TYPE scalar) { \
		return Vec<3, TYPE, proctype>(v.x opr scalar, v.y opr scalar, v.z opr scalar); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<3, TYPE, proctype> operator opr (TYPE scalar, Vec<3, TYPE, proctype> const& v) { \
		return Vec<3, TYPE, proctype>(scalar opr v.x, scalar opr v.y, scalar opr v.z); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<3, TYPE, proctype> operator opr (Vec<3, TYPE, proctype> const& v1, Vec<3, TYPE, proctype> const& v2) {\
		return Vec<3, TYPE, proctype>(v1.x opr v2.x, v1.y opr v2.y, v1.z opr v2.z); \
	} \

template<typename TYPE>
struct Vec<3, TYPE, normal>
{
	typedef TYPE value_type;
	typedef Vec<3, TYPE, normal> class_type;

#if defined(MATH_CONFIG_XYZW_ONLY)
	TYPE x, y, z;
#else
	union { 
		struct { TYPE x, y, z; }; 
		struct { TYPE r, g, b; }; 
		struct { TYPE s, t, p; }; 
		TYPE data[3];
		Vec<2, TYPE, normal> xy;
		Vec<2, TYPE, normal> rg;
		Vec<2, TYPE, normal> st;
	};
#endif
	FORCEINLINE  Vec() : x(static_cast<TYPE>(0)), y(static_cast<TYPE>(0)), z(static_cast<TYPE>(0))
	{}

	FORCEINLINE explicit Vec(TYPE x, TYPE y, TYPE z) : x(static_cast<TYPE>(x)), y(static_cast<TYPE>(y)), z(static_cast<TYPE>(z))
	{}

	FORCEINLINE ~Vec() {}

	//Vec(const Vec& v) = default;
	//Vec(Vec&& v) = default;
	DEFINE_UNARY_OPERATOR(=, 3, normal);
	DEFINE_UNARY_OPERATOR(+=, 3, normal);
	DEFINE_UNARY_OPERATOR(-=, 3, normal);
	DEFINE_UNARY_OPERATOR(*=, 3, normal);
	DEFINE_UNARY_OPERATOR(/=, 3, normal);
	DEFINE_UNARY_OPERATOR(%=, 3, normal);
	DEFINE_UNARY_OPERATOR(&=, 3, normal);
	DEFINE_UNARY_OPERATOR(|=, 3, normal);
	DEFINE_UNARY_OPERATOR(^=, 3, normal);
	DEFINE_UNARY_OPERATOR(<<=, 3, normal);
	DEFINE_UNARY_OPERATOR(>>=, 3, normal);

	FORCEINLINE Vec<3, TYPE, normal>& operator++(void);	//prefix++: no parameter, returns a reference
	FORCEINLINE Vec<3, TYPE, normal> operator++(int);		//postfix++: dummy parameter, returns a value

	FORCEINLINE Vec<3, TYPE, normal>& operator--(void);	//prefix--: no parameter, returns a reference
	FORCEINLINE Vec<3, TYPE, normal> operator--(int);		//postfix--: dummy parameter, returns a value	

	///**********OTHER OPERATIONS***********///

	FORCEINLINE const Vec<3, TYPE, normal> cross_product(const class_type&) const;

	FORCEINLINE TYPE dot_product(const class_type&) const;
	FORCEINLINE TYPE angle(const class_type&) const;

	FORCEINLINE const Vec<3, TYPE, normal> projection(const class_type&) const;
	FORCEINLINE const TYPE scalar_projection(const class_type& v) const;

	FORCEINLINE TYPE lengthSqr() const;
	FORCEINLINE TYPE length() const;
	FORCEINLINE TYPE distance(const class_type&) const;

	FORCEINLINE const Vec<3, TYPE, normal> unit() const;

	
};

template<typename T>
FORCEINLINE Vec<3, T, normal> operator~(Vec<3, T, normal> const& v);

DEFINE_BINARY_OPERATOR(+, 3, normal)
DEFINE_BINARY_OPERATOR(-, 3, normal)
DEFINE_BINARY_OPERATOR(*, 3, normal)
DEFINE_BINARY_OPERATOR(/ , 3, normal)
DEFINE_BINARY_OPERATOR(%, 3, normal)
DEFINE_BINARY_OPERATOR(&, 3, normal)
DEFINE_BINARY_OPERATOR(| , 3, normal)
DEFINE_BINARY_OPERATOR(^, 3, normal)
DEFINE_BINARY_OPERATOR(<< , 3, normal)
DEFINE_BINARY_OPERATOR(>> , 3, normal)

FORCEINLINE Vec<3, bool, normal> operator&&(const Vec<3, bool, normal>&, const Vec<3, bool, normal>&);
FORCEINLINE Vec<3, bool, normal> operator||(const Vec<3, bool, normal>&, const Vec<3, bool, normal>&);

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<3, TYPE, normal>&, const Vec<3, TYPE, normal>&);

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<3, TYPE, normal>&, const Vec<3, TYPE, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<3, float, normal>&, const Vec<3, float, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<3, float, normal>&, const Vec<3, float, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<3, double, normal>&, const Vec<3, double, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<3, double, normal>&, const Vec<3, double, normal>&);

TRE_NS_END

#include "Vec3.inl"
