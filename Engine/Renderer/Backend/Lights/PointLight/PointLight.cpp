#include "PointLight.hpp"

TRE_NS_START

PointLight::PointLight()
{
	SetType(ILight::POINT);
}

void PointLight::SetPosition(const vec3& pos)
{
	m_LightData.m[0][0] = pos.x;
	m_LightData.m[0][1] = pos.y;
	m_LightData.m[0][2] = pos.z;
}

void PointLight::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

void PointLight::SetConstant(float contant)
{
	m_LightData.m[2][0] = contant;
}

void PointLight::SetLinear(float linear)
{
	m_LightData.m[2][1] = linear;
}

void PointLight::SetQuadratic(float quadratic)
{
	m_LightData.m[2][2] = quadratic;
}

TRE_NS_END