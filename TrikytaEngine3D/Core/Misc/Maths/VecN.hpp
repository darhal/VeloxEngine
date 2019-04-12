#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "Vec.hpp"
#include <initializer_list>

#define DECLARE_BINARY_VECN_OPERATOR(opr) \
	template<uint8 N, typename TYPE, proc_type proctype> \
	static FORCEINLINE Vec<N, TYPE, proctype> operator opr (const Vec<N, TYPE, proctype>& v, TYPE scalar) { \
		TYPE data[N]; \
		for (uint8 i = 0; i < N; i++) {  \
			data[i] = v.data[i] opr scalar;\
		}  \
		return Vec<N, TYPE, proctype>(data); \
	} \
	template<uint8 N, typename TYPE, proc_type proctype> \
	static FORCEINLINE Vec<N, TYPE, proctype> operator opr (TYPE scalar, Vec<N, TYPE, proctype> const& v) { \
		TYPE data[N]; \
		for (uint8 i = 0; i < N; i++) {  \
			data[i] = scalar opr v.data[i];\
		}  \
		return Vec<N, TYPE, proctype>(data); \
	} \
	template<uint8 N, typename TYPE, proc_type proctype> \
	static FORCEINLINE Vec<N, TYPE, proctype> operator opr (Vec<N, TYPE, proctype> const& v1, Vec<N, TYPE, proctype> const& v2) {		\
		TYPE data[N]; \
		for (uint8 i = 0; i < N; i++) {  \
			data[i] = v1.data[i] opr v2.data[i];\
		}  \
		return Vec<N, TYPE, proctype>(data); \
	} \

#define DECLARE_UNARY_VECN_OPERATOR(opr) \
	template<typename U>  \
	FORCEINLINE Vec<N, TYPE, Q>& operator opr (const Vec<N, U, Q>& v) { \
		for (uint8 i = 0; i < N; i++) { \
			this->data[i] opr static_cast<TYPE>(v.data[i]); \
		} \
		return *this;   \
	};  \
	template<typename U> \
	FORCEINLINE Vec<N, TYPE, Q>& operator opr (U scalar) { \
		for (uint8 i = 0; i < N; i++) { \
			this->data[i] opr static_cast<TYPE>(scalar); \
		} \
		return *this; \
	}; \

template<uint8 N, typename TYPE, proc_type Q>
struct Vec
{

public:
	typedef TYPE value_type;
	typedef Vec<N, TYPE, Q> class_type;

	TYPE data[N];

	Vec()
	{
		for (uint32 i = 0; i < N; i++) {
			data[i] = 0;
		}
	}

	Vec(TYPE* v, uint32 size)
	{
		for (uint32 i = 0; i < size; i++) {
			data[i] = v[i];
		}
		for (uint32 i = size; i < N; i++) {
			data[i] = 0;
		}
	}

	Vec(TYPE* v)
	{
		for (uint32 i = 0; i < N; i++) {
			data[i] = v[i];
		}
	}

	Vec(const std::initializer_list<TYPE>& list)
	{
		uint8 i = 0;
		for (TYPE elm : list) {
			data[i] = elm;
			i++;
		}
	}

	FORCEINLINE Vec<N, TYPE, Q>& operator++(void)  //prefix++: no parameter, returns a reference
	{
		for (uint32 i = 0; i < N; i++) {
			++data[i];
		}
		return *this;
	}

	FORCEINLINE Vec<N, TYPE, Q> operator++(int) //postfix++: dummy parameter, returns a value
	{
		for (uint32 i = 0; i < N; i++) {
			data[i]++;
		}
		return *this;
	}

	FORCEINLINE Vec<N, TYPE, Q>& operator--(void) //prefix--: no parameter, returns a reference
	{
		for (uint32 i = 0; i < N; i++) {
			--data[i];
		}
		return *this;
	}


	FORCEINLINE Vec<N, TYPE, Q> operator--(int)	//postfix--: dummy parameter, returns a value	
	{
		for (uint32 i = 0; i < N; i++) {
			data[i]--;
		}
		return *this;
	}

	FORCEINLINE const Vec<N, TYPE, Q> projection(const class_type& v) const
	{
		return (dot_product(v) / lengthSqr()) * (*this);
	}

	FORCEINLINE const TYPE scalar_projection(const class_type& v) const
	{
		return (dot_product(v) / length());
	}

	FORCEINLINE TYPE dot_product(const class_type& v) const
	{
		TYPE res = 0;
		for (int i = 0; i < N; i++) {
			res += data[i] * v.data[i];
		}
		return res;
	}

	FORCEINLINE TYPE angle(const class_type& v) const
	{
		return acos(dot_product(v) / length() / v.length());
	}

	FORCEINLINE TYPE lengthSqr() const
	{
		TYPE res = 0;
		for (int i = 0; i < N; i++) {
			res += data[i] * data[i];
		}
		return res;
	}

	FORCEINLINE TYPE length() const
	{
		return sqrt(lengthSqr());
	}

	FORCEINLINE TYPE distance(const class_type& v) const
	{
		return (*this - v).length();
	}

	FORCEINLINE const Vec<N, TYPE, Q> unit() const
	{
		return *this / length();
	}

	DECLARE_UNARY_VECN_OPERATOR(=)
	DECLARE_UNARY_VECN_OPERATOR(+=)
	DECLARE_UNARY_VECN_OPERATOR(-=)
	DECLARE_UNARY_VECN_OPERATOR(*=)
	DECLARE_UNARY_VECN_OPERATOR(/=)
	DECLARE_UNARY_VECN_OPERATOR(%=)
	DECLARE_UNARY_VECN_OPERATOR(&=)
	DECLARE_UNARY_VECN_OPERATOR(|=)
	DECLARE_UNARY_VECN_OPERATOR(^=)
	DECLARE_UNARY_VECN_OPERATOR(<<=)
	DECLARE_UNARY_VECN_OPERATOR(>>=)
};

DECLARE_BINARY_VECN_OPERATOR(+)
DECLARE_BINARY_VECN_OPERATOR(-)
DECLARE_BINARY_VECN_OPERATOR(*)
DECLARE_BINARY_VECN_OPERATOR(/)
DECLARE_BINARY_VECN_OPERATOR(%)
DECLARE_BINARY_VECN_OPERATOR(&)
DECLARE_BINARY_VECN_OPERATOR(|)
DECLARE_BINARY_VECN_OPERATOR(^)
DECLARE_BINARY_VECN_OPERATOR(<<)
DECLARE_BINARY_VECN_OPERATOR(>>)
