#include "SpotLight.hpp"

TRE_NS_START

SpotLight::SpotLight()
{
	SetType(ILight::POINT);
}

void SpotLight::SetPosition(const vec3& pos)
{
	m_LightData.m[0][0] = pos.x;
	m_LightData.m[0][1] = pos.y;
	m_LightData.m[0][2] = pos.z;
}

void SpotLight::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

void SpotLight::SetDirection(const vec3& direction)
{
	m_LightData.m[2][0] = direction.x;
	m_LightData.m[2][1] = direction.y;
	m_LightData.m[2][2] = direction.z;
}

void SpotLight::SetConstant(float contant)
{
	m_LightData.m[2][3] = contant;
}

void SpotLight::SetLinear(float linear)
{
	m_LightData.m[3][0] = linear;
}

void SpotLight::SetQuadratic(float quadratic)
{
	m_LightData.m[3][1] = quadratic;
}

void SpotLight::SetCutOff(float cut_off)
{
	m_LightData.m[3][2] = cut_off;
}

void SpotLight::SetOuterCutOff(float outer_cutoff)
{
	m_LightData.m[3][3] = outer_cutoff;
}

TRE_NS_END