#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Renderer/Components/LightComponents/LightTypes.hpp>

TRE_NS_START

struct PointLightComponent : Component<PointLightComponent>
{
public:
	FORCEINLINE PointLightComponent(const PointLightComponent& other) = default;

	FORCEINLINE PointLightComponent();

	FORCEINLINE void SetPosition(const vec3& pos);

	FORCEINLINE void SetLightColor(const vec3& ambient);

	FORCEINLINE void SetConstant(float contant);

	FORCEINLINE void SetLinear(float linear);

	FORCEINLINE void SetQuadratic(float quadratic);
private:
	Mat4f m_LightData;
	FORCEINLINE void SetType(LightType type) { m_LightData.m[0][3] = (float)type; };
};

FORCEINLINE PointLightComponent::PointLightComponent()
{
	SetType(LightType::POINT);
}

FORCEINLINE void PointLightComponent::SetPosition(const vec3& pos)
{
	m_LightData.m[0][0] = pos.x;
	m_LightData.m[0][1] = pos.y;
	m_LightData.m[0][2] = pos.z;
}

FORCEINLINE void PointLightComponent::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

FORCEINLINE void PointLightComponent::SetConstant(float contant)
{
	m_LightData.m[2][0] = contant;
}

FORCEINLINE void PointLightComponent::SetLinear(float linear)
{
	m_LightData.m[2][1] = linear;
}

FORCEINLINE void PointLightComponent::SetQuadratic(float quadratic)
{
	m_LightData.m[2][2] = quadratic;
}

TRE_NS_END
