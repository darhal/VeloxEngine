#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderEngine/Renderer/Frontend/Lights/ILight/ILight.hpp>

TRE_NS_START

class SpotLight : public ILight
{
public:
	SpotLight();

	void SetDirection(const vec3& direction);

	void SetPosition(const vec3& pos);

	void SetLightColor(const vec3& ambient);

	void SetConstant(float contant);

	void SetLinear(float linear);

	void SetQuadratic(float quadratic);

	void SetCutOff(float cut_off);

	void SetOuterCutOff(float outer_cutoff);
private:

};

TRE_NS_END