#pragma once

#include "Common.hpp"
#include "Vec.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"

TRE_NS_START

#define DECLARE_UNARY_VEC4_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	template<typename U, proc_type P> \
	FORCEINLINE Vec<4, TYPE, proctype>& Vec<4, TYPE, proctype>::operator opr (const Vec<4, U, P>& v){  \
		this->x opr static_cast<TYPE>(v.x);  \
		this->y opr static_cast<TYPE>(v.y);  \
		this->z opr static_cast<TYPE>(v.z);  \
		this->w opr static_cast<TYPE>(v.w);  \
		return *this;  \
	}  \
	template<typename TYPE>  \
	template<typename U>  \
	FORCEINLINE Vec<4, TYPE, proctype>& Vec<4, TYPE, proctype>::operator opr (U scalar){  \
		this->x opr static_cast<TYPE>(scalar);  \
		this->y opr static_cast<TYPE>(scalar);  \
		this->z opr static_cast<TYPE>(scalar);  \
		this->w opr static_cast<TYPE>(scalar);  \
		return *this;  \
	}  \

#define DECLARE_BINARY_VEC4_OPERATOR(opr, proctype) \
	template<typename TYPE> \
	static FORCEINLINE Vec<4, TYPE, proctype> operator opr (const Vec<4, TYPE, proctype>& v, TYPE scalar) { \
		return Vec<4, TYPE, proctype>(v.x opr scalar, v.y opr scalar, v.z opr scalar, v.w opr scalar); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<4, TYPE, proctype> operator opr (TYPE scalar, Vec<4, TYPE, proctype> const& v) { \
		return Vec<4, TYPE, proctype>(scalar opr v.x, scalar opr v.y, scalar opr v.z, scalar opr v.w); \
	} \
	template<typename TYPE> \
	static FORCEINLINE Vec<4, TYPE, proctype> operator opr (Vec<4, TYPE, proctype> const& v1, Vec<4, TYPE, proctype> const& v2) {\
		return Vec<4, TYPE, proctype>(v1.x opr v2.x, v1.y opr v2.y, v1.z opr v2.z, v1.w opr v2.w); \
	} \

template<typename TYPE>
struct Vec<4, TYPE, normal>
{
	typedef TYPE value_type;
	typedef Vec<4, TYPE, normal> class_type;

#if defined(MATH_CONFIG_XYZW_ONLY)
	TYPE x, y, z, w;
#else
	union { 
		struct { TYPE x, y, z, w; }; 
		struct { TYPE r, g, b, a; }; 
		struct { TYPE s, t, p, q; }; 
		TYPE data[4]; 
		struct { Vec<2, TYPE, normal> xy, zw; } pos;
		struct { Vec<2, TYPE, normal> rg, ba; } col;
		struct { Vec<2, TYPE, normal> st, pq; } tex;
		Vec<3, TYPE, normal> xyz;
		Vec<3, TYPE, normal> rga;
		Vec<3, TYPE, normal> stp;
	};
#endif
	FORCEINLINE  Vec() : x(static_cast<TYPE>(0)), y(static_cast<TYPE>(0)), z(static_cast<TYPE>(0)), w(static_cast<TYPE>(0))
	{}

	FORCEINLINE explicit Vec(TYPE x, TYPE y, TYPE z, TYPE w) : x(static_cast<TYPE>(x)), y(static_cast<TYPE>(y)), z(static_cast<TYPE>(z)), w(static_cast<TYPE>(w))
	{}

	FORCEINLINE  ~Vec(){}

	//Vec(const Vec& v) = default;
	//Vec(Vec&& v) = default;
	DEFINE_UNARY_OPERATOR(=, 4, normal);
	DEFINE_UNARY_OPERATOR(+=, 4, normal);
	DEFINE_UNARY_OPERATOR(-=, 4, normal);
	DEFINE_UNARY_OPERATOR(*=, 4, normal);
	DEFINE_UNARY_OPERATOR(/=, 4, normal);
	DEFINE_UNARY_OPERATOR(%=, 4, normal);
	DEFINE_UNARY_OPERATOR(&=, 4, normal);
	DEFINE_UNARY_OPERATOR(|=, 4, normal);
	DEFINE_UNARY_OPERATOR(^=, 4, normal);
	DEFINE_UNARY_OPERATOR(<<=, 4, normal);
	DEFINE_UNARY_OPERATOR(>>=, 4, normal);

	FORCEINLINE Vec<4, TYPE, normal>& operator++(void);	//prefix++: no parameter, returns a reference
	FORCEINLINE Vec<4, TYPE, normal> operator++(int);		//postfix++: dummy parameter, returns a value

	FORCEINLINE Vec<4, TYPE, normal>& operator--(void);	//prefix--: no parameter, returns a reference
	FORCEINLINE Vec<4, TYPE, normal> operator--(int);		//postfix--: dummy parameter, returns a value	

	///**********OTHER OPERATIONS***********///

	FORCEINLINE const Vec<4, TYPE, normal> projection(const class_type& v) const;
	FORCEINLINE const TYPE scalar_projection(const class_type& v) const;

	FORCEINLINE TYPE dot_product(const class_type&) const;
	FORCEINLINE TYPE angle(const class_type&) const;

	FORCEINLINE TYPE lengthSqr() const;
	FORCEINLINE TYPE length() const;
	FORCEINLINE TYPE distance(const class_type&) const;
	FORCEINLINE const Vec<4, TYPE, normal> unit() const;
};

typedef Vec<4, float, normal> vec4;
typedef Vec<4, float, normal> Vec4f;

template<typename T>
FORCEINLINE Vec<4, T, normal> operator~(Vec<4, T, normal> const& v);

DEFINE_BINARY_OPERATOR(+, 4, normal)
DEFINE_BINARY_OPERATOR(-, 4, normal)
DEFINE_BINARY_OPERATOR(*, 4, normal)
DEFINE_BINARY_OPERATOR(/ , 4, normal)
DEFINE_BINARY_OPERATOR(%, 4, normal)
DEFINE_BINARY_OPERATOR(&, 4, normal)
DEFINE_BINARY_OPERATOR(| , 4, normal)
DEFINE_BINARY_OPERATOR(^, 4, normal)
DEFINE_BINARY_OPERATOR(<< , 4, normal)
DEFINE_BINARY_OPERATOR(>> , 4, normal)

FORCEINLINE Vec<4, bool, normal> operator&&(const Vec<4, bool, normal>&, const Vec<4, bool, normal>&);
FORCEINLINE Vec<4, bool, normal> operator||(const Vec<4, bool, normal>&, const Vec<4, bool, normal>&);

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<4, TYPE, normal>&, const Vec<4, TYPE, normal>&);

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<4, TYPE, normal>&, const Vec<4, TYPE, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<4, float, normal>&, const Vec<4, float, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<4, float, normal>&, const Vec<4, float, normal>&);

template<>
FORCEINLINE bool operator==(const Vec<4, double, normal>&, const Vec<4, double, normal>&);

template<>
FORCEINLINE bool operator!=(const Vec<4, double, normal>&, const Vec<4, double, normal>&);

TRE_NS_END

#include "Vec4.inl"

