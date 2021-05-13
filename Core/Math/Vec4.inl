#include "Vec.hpp"
#include "Vec4.hpp"

TRE_NS_START

template<typename TYPE>
FORCEINLINE Vec<4, TYPE, normal>& Vec<4, TYPE, normal>::operator++(void)
{
	++this->x;
	++this->y;
	++this->z;
	++this->w;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<4, TYPE, normal> Vec<4, TYPE, normal>::operator++(int)
{
	this->x++;
	this->y++;
	this->z++;
	this->w++;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<4, TYPE, normal>& Vec<4, TYPE, normal>::operator--(void)
{
	--this->x;
	--this->y;
	--this->z;
	--this->w;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<4, TYPE, normal> Vec<4, TYPE, normal>::operator--(int)
{
	this->x--;
	this->y--;
	this->z--;
	this->w--;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<4, TYPE, normal> operator~(Vec<4, TYPE, normal> const& v)
{
	return Vec<4, TYPE, normal>(~v.x, ~v.y, ~v.z, ~v.w);
}

FORCEINLINE Vec<4, bool, normal> operator&&(Vec<4, bool, normal> const& v1, Vec<4, bool, normal> const& v2)
{
	return Vec<4, bool, normal>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
}

FORCEINLINE Vec<4, bool, normal> operator||(Vec<4, bool, normal> const& v1, Vec<4, bool, normal> const& v2)
{
	return Vec<4, bool, normal>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
}

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<4, TYPE, normal>& v1, const Vec<4, TYPE, normal>& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<4, TYPE, normal>& v1, const Vec<4, TYPE, normal>& v2)
{
	return !(v1 == v2) && !(v1.y == v2.y) && !(v1.z == v2.z) && !(v1.w == v2.w);
}

template<>
FORCEINLINE bool operator==(const Vec<4, float, normal>& v1, const Vec<4, float, normal>& v2)
{
	return (Math::FloatEqual(v1.x, v2.x) && Math::FloatEqual(v1.y, v2.y) && Math::FloatEqual(v1.z, v2.z) && Math::FloatEqual(v1.w, v2.w));
}

template<>
FORCEINLINE bool operator!=(const Vec<4, float, normal>& v1, const Vec<4, float, normal>& v2)
{
	return !Math::FloatEqual(v1.x, v2.x) && !Math::FloatEqual(v1.y, v2.y) && !Math::FloatEqual(v1.z, v2.z) && !Math::FloatEqual(v1.w, v2.w);
}

template<>
FORCEINLINE bool operator==(const Vec<4, double, normal>& v1, const Vec<4, double, normal>& v2)
{
	return Math::FloatEqual(static_cast<float>(v1.x), static_cast<float>(v2.x)) &&
		Math::FloatEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)) && 
		Math::FloatEqual(static_cast<float>(v1.z), static_cast<float>(v2.z)) &&
		Math::FloatEqual(static_cast<float>(v1.w), static_cast<float>(v2.w));
}

template<>
FORCEINLINE bool operator!=(const Vec<4, double, normal>& v1, const Vec<4, double, normal>& v2)
{
	return !Math::FloatEqual(static_cast<float>(v1.x), static_cast<float>(v2.x)) 
		&& !Math::FloatEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)) 
		&& !Math::FloatEqual(static_cast<float>(v1.z), static_cast<float>(v2.z))
		&& !Math::FloatEqual(static_cast<float>(v1.w), static_cast<float>(v2.w));
}


DECLARE_UNARY_VEC4_OPERATOR(=, normal)
DECLARE_UNARY_VEC4_OPERATOR(+=, normal)
DECLARE_UNARY_VEC4_OPERATOR(-=, normal)
DECLARE_UNARY_VEC4_OPERATOR(*=, normal)
DECLARE_UNARY_VEC4_OPERATOR(/=, normal)
DECLARE_UNARY_VEC4_OPERATOR(%=, normal)
DECLARE_UNARY_VEC4_OPERATOR(&=, normal)
DECLARE_UNARY_VEC4_OPERATOR(|=, normal)
DECLARE_UNARY_VEC4_OPERATOR(^=, normal)
DECLARE_UNARY_VEC4_OPERATOR(<<=, normal)
DECLARE_UNARY_VEC4_OPERATOR(>>=, normal)

DECLARE_BINARY_VEC4_OPERATOR(+, normal)
DECLARE_BINARY_VEC4_OPERATOR(-, normal)
DECLARE_BINARY_VEC4_OPERATOR(*, normal)
DECLARE_BINARY_VEC4_OPERATOR(/ , normal)
DECLARE_BINARY_VEC4_OPERATOR(%, normal)
DECLARE_BINARY_VEC4_OPERATOR(&, normal)
DECLARE_BINARY_VEC4_OPERATOR(| , normal)
DECLARE_BINARY_VEC4_OPERATOR(^, normal)
DECLARE_BINARY_VEC4_OPERATOR(<< , normal)
DECLARE_BINARY_VEC4_OPERATOR(>> , normal)

template<typename TYPE>
FORCEINLINE TYPE Vec<4, TYPE, normal>::dot_product(const class_type& v) const
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<4, TYPE, normal>::angle(const class_type& v) const
{
	return acos(dot_product(v) / length() / v.length());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<4, TYPE, normal>::lengthSqr() const
{
	return x * x + y * y + z * z + w * w;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<4, TYPE, normal>::length() const
{
	return sqrt(lengthSqr());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<4, TYPE, normal>::distance(const class_type& v) const
{
	return (*this - v).length();
}

template<typename TYPE>
FORCEINLINE const Vec<4, TYPE, normal> Vec<4, TYPE, normal>::unit() const
{
	return *this / length();
}

template<typename TYPE>
FORCEINLINE const Vec<4, TYPE, normal> Vec<4, TYPE, normal>::projection(const class_type& v) const
{
	return (dot_product(v) / lengthSqr()) * (*this);
}

template<typename TYPE>
FORCEINLINE const TYPE Vec<4, TYPE, normal>::scalar_projection(const class_type& v) const
{
	return (dot_product(v) / length());
}


TRE_NS_END


