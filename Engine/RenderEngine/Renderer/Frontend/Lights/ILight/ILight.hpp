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

	void SetType(LightType type) { m_LightData.m[0][3] = (float) type; };

protected:
	Mat4f m_LightData;
};

TRE_NS_END