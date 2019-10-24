#include "Vec.hpp"
#include "Vec2.hpp"

TRE_NS_START

template<typename TYPE>
FORCEINLINE Vec<2, TYPE, normal>& Vec<2, TYPE, normal>::operator++(void)
{
	++this->x;
	++this->y;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<2, TYPE, normal> Vec<2, TYPE, normal>::operator++(int)
{
	this->x++;
	this->y++;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<2, TYPE, normal>& Vec<2, TYPE, normal>::operator--(void)
{
	--this->x;
	--this->y;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<2, TYPE, normal> Vec<2, TYPE, normal>::operator--(int)
{
	this->x--;
	this->y--;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<2, TYPE, normal> operator~(Vec<2, TYPE, normal> const& v)
{
	return Vec<2, TYPE, normal>(~v.x, ~v.y);
}

FORCEINLINE Vec<2, bool, normal> operator&&(Vec<2, bool, normal> const& v1, Vec<2, bool, normal> const& v2)
{
	return Vec<2, bool, normal>(v1.x && v2.x, v1.y && v2.y);
}

FORCEINLINE Vec<2, bool, normal> operator||(Vec<2, bool, normal> const& v1, Vec<2, bool, normal> const& v2)
{
	return Vec<2, bool, normal>(v1.x || v2.x, v1.y || v2.y);
}

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<2, TYPE, normal>& v1, const Vec<2, TYPE, normal>& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<2, TYPE, normal>& v1, const Vec<2, TYPE, normal>& v2)
{
	return !(v1 == v2) && !(v1.y == v2.y);
}

template<>
FORCEINLINE bool operator==(const Vec<2, float, normal>& v1, const Vec<2, float, normal>& v2)
{
	return (Math::FloatEqual(v1.x, v2.x) && Math::FloatEqual(v1.y, v2.y));
}

template<>
FORCEINLINE bool operator!=(const Vec<2, float, normal>& v1, const Vec<2, float, normal>& v2)
{
	return !Math::FloatEqual(v1.x, v2.x) && !Math::FloatEqual(v1.y, v2.y);
}

template<>
FORCEINLINE bool operator==(const Vec<2, double, normal>& v1, const Vec<2, double, normal>& v2)
{
	return (Math::FloatEqual(static_cast<float>(v1.x), static_cast<float>(v2.x)) && Math::FloatEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)));
}

template<>
FORCEINLINE bool operator!=(const Vec<2, double, normal>& v1, const Vec<2, double, normal>& v2)
{
	return !(Math::FloatEqual(static_cast<float>(v1.x), static_cast<float>(v2.x))) && !(Math::FloatEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)));
}

DECLARE_UNARY_VEC2_OPERATOR(=, normal)
DECLARE_UNARY_VEC2_OPERATOR(+=, normal)
DECLARE_UNARY_VEC2_OPERATOR(-=, normal)
DECLARE_UNARY_VEC2_OPERATOR(*=, normal)
DECLARE_UNARY_VEC2_OPERATOR(/=, normal)
DECLARE_UNARY_VEC2_OPERATOR(%=, normal)
DECLARE_UNARY_VEC2_OPERATOR(&=, normal)
DECLARE_UNARY_VEC2_OPERATOR(|=, normal)
DECLARE_UNARY_VEC2_OPERATOR(^=, normal)
DECLARE_UNARY_VEC2_OPERATOR(<<=, normal)
DECLARE_UNARY_VEC2_OPERATOR(>>=, normal)

DECLARE_BINARY_VEC2_OPERATOR(+, normal)
DECLARE_BINARY_VEC2_OPERATOR(-, normal)
DECLARE_BINARY_VEC2_OPERATOR(*, normal)
DECLARE_BINARY_VEC2_OPERATOR(/ , normal)
DECLARE_BINARY_VEC2_OPERATOR(%, normal)
DECLARE_BINARY_VEC2_OPERATOR(&, normal)
DECLARE_BINARY_VEC2_OPERATOR(| , normal)
DECLARE_BINARY_VEC2_OPERATOR(^, normal)
DECLARE_BINARY_VEC2_OPERATOR(<< , normal)
DECLARE_BINARY_VEC2_OPERATOR(>> , normal)

template<typename TYPE>
FORCEINLINE const TYPE Vec<2, TYPE, normal>::cross_product(const class_type& v) const
{
	return (x * v.y - y * v.x);
}

template<typename TYPE>
FORCEINLINE TYPE Vec<2, TYPE, normal>::dot_product(const class_type& v) const
{
	return x * v.x + y * v.y;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<2, TYPE, normal>::angle(const class_type& v) const
{
	return acos(dot_product(v) / length() / v.length());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<2, TYPE, normal>::lengthSqr() const
{
	return x * x + y * y;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<2, TYPE, normal>::length() const
{
	return sqrt(lengthSqr());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<2, TYPE, normal>::distance(const class_type& v) const
{
	return (*this - v).length();
}

template<typename TYPE>
FORCEINLINE const Vec<2, TYPE, normal> Vec<2, TYPE, normal>::unit() const
{
	return *this / length();
}

template<typename TYPE>
FORCEINLINE const Vec<2, TYPE, normal> Vec<2, TYPE, normal>::projection(const class_type& v) const
{
	return (dot_product(v) / lengthSqr()) * (*this);
}

template<typename TYPE>
FORCEINLINE const TYPE Vec<2, TYPE, normal>::scalar_projection(const class_type& v) const
{
	return (dot_product(v) / length());
}

TRE_NS_END