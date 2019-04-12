#include "Vec3.hpp"

template<typename TYPE>
FORCEINLINE Vec<3, TYPE, normal>& Vec<3, TYPE, normal>::operator++(void)
{
	++this->x;
	++this->y;
	++this->z;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<3, TYPE, normal> Vec<3, TYPE, normal>::operator++(int)
{
	this->x++;
	this->y++;
	this->z++;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<3, TYPE, normal>& Vec<3, TYPE, normal>::operator--(void)
{
	--this->x;
	--this->y;
	--this->z;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<3, TYPE, normal> Vec<3, TYPE, normal>::operator--(int)
{
	this->x--;
	this->y--;
	this->z--;
	return *this;
}

template<typename TYPE>
FORCEINLINE Vec<3, TYPE, normal> operator~(Vec<3, TYPE, normal> const& v)
{
	return Vec<3, TYPE, normal>(~v.x, ~v.y, ~v.z);
}

FORCEINLINE Vec<3, bool, normal> operator&&(Vec<3, bool, normal> const& v1, Vec<3, bool, normal> const& v2)
{
	return Vec<3, bool, normal>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
}

FORCEINLINE Vec<3, bool, normal> operator||(Vec<3, bool, normal> const& v1, Vec<3, bool, normal> const& v2)
{
	return Vec<3, bool, normal>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
}

template<typename TYPE>
FORCEINLINE bool operator==(const Vec<3, TYPE, normal>& v1, const Vec<3, TYPE, normal>& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

template<typename TYPE>
FORCEINLINE bool operator!=(const Vec<3, TYPE, normal>& v1, const Vec<3, TYPE, normal>& v2)
{
	return !(v1 == v2) && !(v1.y == v2.y) && !(v1.z == v2.z);
}

template<>
FORCEINLINE bool operator==(const Vec<3, float, normal>& v1, const Vec<3, float, normal>& v2)
{
	return (isEqual(v1.x, v2.x) && isEqual(v1.y, v2.y) && isEqual(v1.z, v2.z));
}

template<>
FORCEINLINE bool operator!=(const Vec<3, float, normal>& v1, const Vec<3, float, normal>& v2)
{
	return !isEqual(v1.x, v2.x) && !isEqual(v1.y, v2.y) && !isEqual(v1.z, v2.z);
}

template<>
FORCEINLINE bool operator==(const Vec<3, double, normal>& v1, const Vec<3, double, normal>& v2)
{
	return isEqual(static_cast<float>(v1.x), static_cast<float>(v2.x)) && isEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)) && isEqual(static_cast<float>(v1.z), static_cast<float>(v2.z));
}

template<>
FORCEINLINE bool operator!=(const Vec<3, double, normal>& v1, const Vec<3, double, normal>& v2)
{
	return !isEqual(static_cast<float>(v1.x), static_cast<float>(v2.x)) && !isEqual(static_cast<float>(v1.y), static_cast<float>(v2.y)) && !isEqual(static_cast<float>(v1.z), static_cast<float>(v2.z));
}


DECLARE_UNARY_VEC3_OPERATOR(=, normal)
DECLARE_UNARY_VEC3_OPERATOR(+=, normal)
DECLARE_UNARY_VEC3_OPERATOR(-=, normal)
DECLARE_UNARY_VEC3_OPERATOR(*=, normal)
DECLARE_UNARY_VEC3_OPERATOR(/=, normal)
DECLARE_UNARY_VEC3_OPERATOR(%=, normal)
DECLARE_UNARY_VEC3_OPERATOR(&=, normal)
DECLARE_UNARY_VEC3_OPERATOR(|=, normal)
DECLARE_UNARY_VEC3_OPERATOR(^=, normal)
DECLARE_UNARY_VEC3_OPERATOR(<<=, normal)
DECLARE_UNARY_VEC3_OPERATOR(>>=, normal)

DECLARE_BINARY_VEC3_OPERATOR(+, normal)
DECLARE_BINARY_VEC3_OPERATOR(-, normal)
DECLARE_BINARY_VEC3_OPERATOR(*, normal)
DECLARE_BINARY_VEC3_OPERATOR(/ , normal)
DECLARE_BINARY_VEC3_OPERATOR(%, normal)
DECLARE_BINARY_VEC3_OPERATOR(&, normal)
DECLARE_BINARY_VEC3_OPERATOR(| , normal)
DECLARE_BINARY_VEC3_OPERATOR(^, normal)
DECLARE_BINARY_VEC3_OPERATOR(<< , normal)
DECLARE_BINARY_VEC3_OPERATOR(>> , normal)

template<typename TYPE>
FORCEINLINE const Vec<3, TYPE, normal> Vec<3, TYPE, normal>::cross_product(const class_type& v) const
{
	return class_type(y*v.z - z * v.y, z*v.x - x * v.z, x*v.y - y * v.x);
}

template<typename TYPE>
FORCEINLINE TYPE Vec<3, TYPE, normal>::dot_product(const class_type& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<3, TYPE, normal>::angle(const class_type& v) const
{
	return acos(dot_product(v) / length() / v.length());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<3, TYPE, normal>::lengthSqr() const
{
	return x * x + y * y + z * z;
}

template<typename TYPE>
FORCEINLINE TYPE Vec<3, TYPE, normal>::length() const
{
	return (TYPE)sqrt(lengthSqr());
}

template<typename TYPE>
FORCEINLINE TYPE Vec<3, TYPE, normal>::distance(const class_type& v) const
{
	return (*this - v).length();
}

template<typename TYPE>
FORCEINLINE const Vec<3, TYPE, normal> Vec<3, TYPE, normal>::unit() const
{
	return *this / length();
}

template<typename TYPE>
FORCEINLINE const Vec<3, TYPE, normal> Vec<3, TYPE, normal>::projection(const class_type& v) const
{
	return (dot_product(v) / lengthSqr()) * (*this);
}

template<typename TYPE>
FORCEINLINE const TYPE Vec<3, TYPE, normal>::scalar_projection(const class_type& v) const
{
	return (dot_product(v) / length());
}

