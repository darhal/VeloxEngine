//#include "Quaternion.hpp"
//
//TRE_NS_START
//
//// From Ken Shoemake's "Quaternion Calculus and Fast Animation" article
//Quaternion::Quaternion(const Mat4f& rot)
//{
//	float trace = rot[0][0] + rot[1][1] + rot[2][2];
//
//	if (trace > 0)
//	{
//		float s = 0.5f / (float)Math::Sqrt(trace + 1.0f);
//		coord = Vec4f(
//			(rot[1][2] - rot[2][1]) * s,
//			(rot[2][0] - rot[0][2]) * s,
//			(rot[0][1] - rot[1][0]) * s,
//			0.25f / s
//		);
//	}
//	else {
//		if (rot[0][0] > rot[1][1] && rot[0][0] > rot[2][2]) {
//			float s = 2.0f * (float)Math::Sqrt(1.0f + rot[0][0] - rot[1][1] - rot[2][2]);
//			coord = Vec4f(0.25f * s,
//				(rot[1][0] + rot[0][1]) / s,
//				(rot[2][0] + rot[0][2]) / s,
//				(rot[1][2] - rot[2][1]) / s
//			);
//		}
//		else if (rot[1][1] > rot[2][2]) {
//			float s = 2.0f * (float)Math::Sqrt(1.0f + rot[1][1] - rot[0][0] - rot[2][2]);
//			coord = Vec4f(
//				(rot[1][0] + rot[0][1]) / s,
//				0.25f * s,
//				(rot[2][1] + rot[1][2]) / s,
//				(rot[2][0] - rot[0][2]) / s
//			);
//		}
//		else {
//			float s = 2.0f * (float)Math::Sqrt(1.0f + rot[2][2] - rot[0][0] - rot[1][1]);
//			coord = Vec4f(
//				(rot[2][0] + rot[0][2]) / s,
//				(rot[1][2] + rot[2][1]) / s,
//				(rot[0][1] + rot[1][0]) / s,
//				0.25f * s
//			);
//		}
//	}
//
//	float length = coord.length();
//	coord /= length;
//}
//
//Mat4f Quaternion::ToRotationMatrix()
//{
//	Vec4f f(2.0f * (coord.x * coord.z - coord.w * coord.y), 2.0f * (coord.y * coord.z + coord.w * coord.x), 1.0f - 2.0f * (coord.x * coord.x + coord.y * coord.y), 0.f);
//	Vec4f u(2.0f * (coord.x * coord.y + coord.w * coord.z), 1.0f - 2.0f * (coord.x * coord.x + coord.z * coord.z), 2.0f * (coord.y * coord.z - coord.w * coord.x), 0.f);
//	Vec4f r(1.0f - 2.0f * (coord.y * coord.y + coord.z * coord.z), 2.0f * (coord.x * coord.y - coord.w * coord.z), 2.0f * (coord.x * coord.z + coord.w * coord.y), 0.f);
//
//	return Mat4f(r, u, f, Vec4f(0.f, 0.f, 0.f, 1.f));
//}
//
//Quaternion Quaternion::NLerp(const Quaternion& dest, float lerpFactor, bool shortest)
//{
//	Quaternion correctedDest = dest;
//
//	if (shortest && this->Dot(dest) < 0)
//		correctedDest = Quaternion(-dest.x, -dest.y, -dest.z, -dest.w);
//
//	return (((correctedDest - *this) * lerpFactor) + *this).Normalize();
//}
//
//Quaternion Quaternion::SLerp(const Quaternion& dest, float lerpFactor, bool shortest)
//{
//	CONSTEXPR float EPSILON = 1e3f;
//
//	float cos = this->Dot(dest);
//	Quaternion correctedDest = dest;
//
//	if (shortest && cos < 0)
//	{
//		cos = -cos;
//		correctedDest = Quaternion(-dest.x, -dest.y, -dest.z, -dest.w);
//	}
//
//	if (Math::Abs(cos) >= 1 - EPSILON)
//		return NLerp(correctedDest, lerpFactor, false);
//
//	float sin = (float)Math::Sqrt(1.0f - cos * cos);
//	float angle = (float)Math::Atan2(sin, cos);
//	float invSin = 1.0f / sin;
//
//	float srcFactor = (float)Math::Sin((1.0f - lerpFactor) * angle) * invSin;
//	float destFactor = (float)Math::Sin((lerpFactor)* angle) * invSin;
//
//	return (*this * srcFactor) + (correctedDest * destFactor);
//}
//
//
//
//TRE_NS_END