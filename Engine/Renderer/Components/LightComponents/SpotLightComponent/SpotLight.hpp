#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Renderer/Components/LightComponents/LightTypes.hpp>

TRE_NS_START

class SpotLightComponent : Component<SpotLightComponent>
{
public:
	FORCEINLINE SpotLightComponent(const SpotLightComponent& other) = default;

	FORCEINLINE SpotLightComponent();

	FORCEINLINE void SetDirection(const vec3& direction);

	FORCEINLINE void SetPosition(const vec3& pos);

	FORCEINLINE void SetLightColor(const vec3& ambient);

	FORCEINLINE void SetConstant(float contant);

	FORCEINLINE void SetLinear(float linear);

	FORCEINLINE void SetQuadratic(float quadratic);

	FORCEINLINE void SetCutOff(float cut_off);

	FORCEINLINE void SetOuterCutOff(float outer_cutoff);

private:
	Mat4f m_LightData;
	FORCEINLINE void SetType(LightType type) { m_LightData.m[0][3] = (float)type; };
};

FORCEINLINE SpotLightComponent::SpotLightComponent()
{
	SetType(LightType::POINT);
}

FORCEINLINE void SpotLightComponent::SetPosition(const vec3& pos)
{
	m_LightData.m[0][0] = pos.x;
	m_LightData.m[0][1] = pos.y;
	m_LightData.m[0][2] = pos.z;
}

FORCEINLINE void SpotLightComponent::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

FORCEINLINE void SpotLightComponent::SetDirection(const vec3& direction)
{
	m_LightData.m[2][0] = direction.x;
	m_LightData.m[2][1] = direction.y;
	m_LightData.m[2][2] = direction.z;
}

FORCEINLINE void SpotLightComponent::SetConstant(float contant)
{
	m_LightData.m[2][3] = contant;
}

FORCEINLINE void SpotLightComponent::SetLinear(float linear)
{
	m_LightData.m[3][0] = linear;
}

FORCEINLINE void SpotLightComponent::SetQuadratic(float quadratic)
{
	m_LightData.m[3][1] = quadratic;
}

FORCEINLINE void SpotLightComponent::SetCutOff(float cut_off)
{
	m_LightData.m[3][2] = cut_off;
}

FORCEINLINE void SpotLightComponent::SetOuterCutOff(float outer_cutoff)
{
	m_LightData.m[3][3] = outer_cutoff;
}

TRE_NS_END