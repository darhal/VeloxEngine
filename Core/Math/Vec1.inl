#include "Vec.hpp"
#include "Vec1.hpp"

TRE_NS_START

template<typename TYPE>
FORCEINLINE Vec<1, TYPE, normal>& Vec<1, TYPE, normal>::operator++(void)
{
	++this->x;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<1, TYPE, normal> Vec<1, TYPE, normal>::operator++(int)
{
	this->x++;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<1, TYPE, normal>& Vec<1, TYPE, normal>::operator--(void)
{
	--this->x;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<1, TYPE, normal> Vec<1, TYPE, normal>::operator--(int)
{
	this->x--;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<1, TYPE, normal> operator~(Vec<1, TYPE, normal> const& v)
{
	return Vec<1, TYPE, normal>(~v.x);
}

FORCEINLINE Vec<1, bool, normal> operator&&(Vec<1, bool, normal> const& v1, Vec<1, bool, normal> const& v2)
{
	return Vec<1, bool, normal>(v1.x && v2.x);
}

FORCEINLINE Vec<1, bool, normal> operator||(Vec<1, bool, normal> const& v1, Vec<1, bool, normal> const& v2)
{
	return Vec<1, bool, normal>(v1.x || v2.x);
}

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<1, TYPE, normal>& v1, const Vec<1, TYPE, normal>& v2)
{
	return (v1.x == v2.x);
}

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<1, TYPE, normal>& v1, const Vec<1, TYPE, normal>& v2)
{
	return !(v1 == v2);
}

template<>
FORCEINLINE bool operator==(const Vec<1, float, normal>& v1, const Vec<1, float, normal>& v2)
{
	return (Math::FloatEqual(v1.x, v2.x));
}

template<>
FORCEINLINE bool operator!=(const Vec<1, float, normal>& v1, const Vec<1, float, normal>& v2)
{
	return !(Math::FloatEqual(v1.x, v2.x));
}

template<>
FORCEINLINE bool operator==(const Vec<1, double, normal>& v1, const Vec<1, double, normal>& v2)
{
	return (Math::FloatEqual(v1.x, v2.x));
}

template<>
FORCEINLINE bool operator!=(const Vec<1, double, normal>& v1, const Vec<1, double, normal>& v2)
{
	return !(Math::FloatEqual(v1.x, v2.x));
}

DECLARE_UNARY_VEC1_OPERATOR(=, normal)
DECLARE_UNARY_VEC1_OPERATOR(+=, normal)
DECLARE_UNARY_VEC1_OPERATOR(-=, normal)
DECLARE_UNARY_VEC1_OPERATOR(*=, normal)
DECLARE_UNARY_VEC1_OPERATOR(/=, normal)
DECLARE_UNARY_VEC1_OPERATOR(%=, normal)
DECLARE_UNARY_VEC1_OPERATOR(&=, normal)
DECLARE_UNARY_VEC1_OPERATOR(|=, normal)
DECLARE_UNARY_VEC1_OPERATOR(^=, normal)
DECLARE_UNARY_VEC1_OPERATOR(<<=, normal)
DECLARE_UNARY_VEC1_OPERATOR(>>=, normal)

DECLARE_BINARY_VEC1_OPERATOR(+, normal)
DECLARE_BINARY_VEC1_OPERATOR(-, normal)
DECLARE_BINARY_VEC1_OPERATOR(*, normal)
DECLARE_BINARY_VEC1_OPERATOR(/ , normal)
DECLARE_BINARY_VEC1_OPERATOR(%, normal)
DECLARE_BINARY_VEC1_OPERATOR(&, normal)
DECLARE_BINARY_VEC1_OPERATOR(| , normal)
DECLARE_BINARY_VEC1_OPERATOR(^, normal)
DECLARE_BINARY_VEC1_OPERATOR(<< , normal)
DECLARE_BINARY_VEC1_OPERATOR(>> , normal)

TRE_NS_END


/*template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator=(const Vec<1, U>& v)
{
	this->x = static_cast<TYPE>(v.x);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator=(U scalar)
{
	this->x = static_cast<TYPE>(scalar);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE> & Vec<1, TYPE>::operator+=(const Vec<1, U>& v)
{
	this->x += static_cast<TYPE>(v.x);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator+=(U scalar)
{
	this->x += static_cast<TYPE>(scalar);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE> & Vec<1, TYPE>::operator/=(const Vec<1, U>& v)
{
	this->x /= static_cast<TYPE>(v.x);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator/=(U scalar)
{
	this->x /= static_cast<TYPE>(scalar);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE> & Vec<1, TYPE>::operator*=(const Vec<1, U>& v)
{
	this->x *= static_cast<TYPE>(v.x);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator*=(U scalar)
{
	this->x *= static_cast<TYPE>(scalar);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE> & Vec<1, TYPE>::operator-=(const Vec<1, U>& v)
{
	this->x -= static_cast<TYPE>(v.x);
	return *this;
}

template<typename TYPE>
template<typename U>
FORCEINLINE Vec<1, TYPE>& Vec<1, TYPE>::operator-=(U scalar)
{
	this->x -= static_cast<TYPE>(scalar);
	return *this;
}*/