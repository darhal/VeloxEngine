#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Components/LightComponents/DirectionalLightComponent/DirectionalLight.hpp>
#include <Core/ECS/System/BaseSystem.hpp>

TRE_NS_START

class DirectionalLightSystem : public System<DirectionalLightComponent>
{
public:
	void UpdateComponents(float dt, Archetype& arche) final;
private:

};

TRE_NS_END