#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderEngine/Renderer/Frontend/Lights/ILight/ILight.hpp>

TRE_NS_START

class DirectionalLight : public ILight
{
public:
	DirectionalLight();

	void SetPosition(const vec3& pos);

	void SetLightColor(const vec3& color);
private:

};

TRE_NS_END