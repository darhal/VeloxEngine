#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Components/LightComponents/DirectionalLightComponent/DirectionalLight.hpp>

TRE_NS_START

class VBO;

class LightSystem
{
public:
	LightSystem();

	void AddLight(Mat4f* comp);
private:
	VBO* m_LightVbo;
	uint32 m_LightCount;
	uint32 m_MaxLight;

	friend class ForwardRenderer;
};

TRE_NS_END