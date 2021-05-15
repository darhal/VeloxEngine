//#pragma once
//
//// https://github.com/BennyQBD/3DGameEngine/blob/master/src/com/base/Core/Quaternion.java
//
//#include <Core/Misc/Defines/Common.hpp>
//#include "Common.hpp"
//#include "Vec4f_simd.hpp"
//#include "Matrix4x4.hpp"
//#include "Maths.hpp"
//
//TRE_NS_START
//
//typedef struct Quaternion Quat;
//
//struct Quaternion
//{
//public:
//	// From Ken Shoemake's "Quaternion Calculus and Fast Animation" article
//	Quaternion(const Mat4f& rot);
//
//	FORCEINLINE Quaternion(float x, float y, float z, float w) : coord(x, y, z, w)
//	{}
//
//	FORCEINLINE Quaternion(const Vec4f& vec) : coord(vec)
//	{}
//
//	FORCEINLINE Quaternion(Vec3f axis, float angle) : 
//		coord(
//			axis.x * Math::Sin(angle / 2), 
//			axis.y * Math::Sin(angle / 2), 
//			axis.z * Math::Sin(angle / 2), 
//			Math::Cos(angle / 2)
//		)
//	{}
//
//	~Quaternion() {};
//
//	FORCEINLINE float Length() const;
//
//	FORCEINLINE Quaternion& Normalize();
//
//	FORCEINLINE Quaternion& Conjugate();
//
//	FORCEINLINE Quaternion Normalize() const;
//
//	FORCEINLINE Quaternion Conjugate() const;
//
//	FORCEINLINE Quaternion operator*(float r);
//
//	FORCEINLINE Quaternion operator*(const Quaternion& r);
//
//	FORCEINLINE Quaternion operator*(const Vec3f& r);
//
//	FORCEINLINE Quaternion operator-(const Quaternion& r);
//
//	FORCEINLINE Quaternion operator+(const Quaternion& r);
//
//	FORCEINLINE float Dot(const Quaternion& r);
//
//	Mat4f ToRotationMatrix();
//
//	Quaternion NLerp(const Quaternion& dest, float lerpFactor, bool shortest);
//
//	Quaternion SLerp(const Quaternion& dest, float lerpFactor, bool shortest);
//
//private:
//	union {
//		Vec4f coord;
//		float x, y, z, w;
//	};
//	
//};
//
//
//FORCEINLINE float Quaternion::Length() const
//{
//	return coord.length();
//}
//
//FORCEINLINE Quaternion& Quaternion::Normalize()
//{
//	float length = this->Length();
//	coord = (coord / length);
//	return *this;
//}
//
//FORCEINLINE Quaternion& Quaternion::Conjugate()
//{
//	coord = Vec4f(-coord.x, -coord.y, -coord.z, coord.w);
//	return *this;
//}
//
//FORCEINLINE Quaternion Quaternion::Normalize() const
//{
//	float length = this->Length();
//	return Quaternion(coord / length);
//}
//
//FORCEINLINE Quaternion Quaternion::Conjugate() const
//{
//	return Quaternion(-coord.x, -coord.y, -coord.z, coord.w);
//}
//
//
//FORCEINLINE Quaternion Quaternion::operator*(float r)
//{
//	return Quaternion(coord * r);
//}
//
//FORCEINLINE Quaternion Quaternion::operator*(const Quaternion& r)
//{
//	float w_ = coord.w * r.coord.w - coord.x * r.coord.x - coord.y * r.coord.y - coord.z * r.coord.z;
//	float x_ = coord.x * r.coord.w + coord.w * r.coord.x + coord.y * r.coord.z - coord.z * r.coord.y;
//	float y_ = coord.y * r.coord.w + coord.w * r.coord.y + coord.z * r.coord.x - coord.x * r.coord.z;
//	float z_ = coord.z * r.coord.w + coord.w * r.coord.z + coord.x * r.coord.y - coord.y * r.coord.x;
//
//	return Quaternion(x_, y_, z_, w_);
//}
//
//FORCEINLINE Quaternion Quaternion::operator*(const Vec3f& r)
//{
//	float w_ = -coord.x * r.x - coord.y * r.y - coord.z * r.z;
//	float x_ = coord.w * r.x + coord.y * r.z - coord.z * r.y;
//	float y_ = coord.w * r.y + coord.z * r.x - coord.x * r.z;
//	float z_ = coord.w * r.z + coord.x * r.y - coord.y * r.x;
//
//	return Quaternion(x_, y_, z_, w_);
//}
//
//FORCEINLINE Quaternion Quaternion::operator-(const Quaternion& r)
//{
//	return Quaternion(coord - r.coord);
//}
//
//FORCEINLINE Quaternion Quaternion::operator+(const Quaternion& r)
//{
//	return Quaternion(coord + r.coord);
//}
//
//FORCEINLINE float Quaternion::Dot(const Quaternion& r)
//{
//	return coord.dot_product(r.coord);
//}
//
//TRE_NS_END