#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Renderer/Components/LightComponents/LightTypes.hpp>

TRE_NS_START

struct DirectionalLightComponent : Component<DirectionalLightComponent>
{
public:
	FORCEINLINE DirectionalLightComponent();

	FORCEINLINE DirectionalLightComponent(const DirectionalLightComponent& other) = default;

	FORCEINLINE void SetDirection(const vec3& dir);

	FORCEINLINE void SetLightColor(const vec3& color);

private:
	Mat4f m_LightData;
	FORCEINLINE void SetType(LightType type) { m_LightData.m[0][3] = (float)type; };
};

FORCEINLINE DirectionalLightComponent::DirectionalLightComponent()
{
	SetType(LightType::DIRECTIONAL);
};

FORCEINLINE void DirectionalLightComponent::SetDirection(const vec3& dir)
{
	m_LightData.m[0][0] = dir.x;
	m_LightData.m[0][1] = dir.y;
	m_LightData.m[0][2] = dir.z;
}

FORCEINLINE void DirectionalLightComponent::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

TRE_NS_END