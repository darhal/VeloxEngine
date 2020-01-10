#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Backend/Lights/ILight/ILight.hpp>

TRE_NS_START

class DirectionalLight : public ILight
{
public:
	DirectionalLight();

	void SetDirection(const vec3& dir);

	void SetLightColor(const vec3& color);
private:

};

TRE_NS_END