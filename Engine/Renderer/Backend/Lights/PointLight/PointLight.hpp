#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Backend/Lights/ILight/ILight.hpp>

TRE_NS_START

class PointLight : public ILight
{
public:
	PointLight();

	void SetPosition(const vec3& pos);

	void SetLightColor(const vec3& ambient);

	void SetConstant(float contant);

	void SetLinear(float linear);

	void SetQuadratic(float quadratic);
private:

};

TRE_NS_END
