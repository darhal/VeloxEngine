#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>

TRE_NS_START

class ILight
{
public:
	enum LightType{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2,
	};

	FORCEINLINE void SetType(LightType type) { m_LightData.m[0][3] = (float) type; };

	FORCEINLINE const Mat4f& GetLightMatrix() const { return m_LightData; };

protected:
	Mat4f m_LightData;
};

TRE_NS_END