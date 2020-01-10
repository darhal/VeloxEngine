#include "DirectionalLight.hpp"

TRE_NS_START

DirectionalLight::DirectionalLight() 
{
	SetType(ILight::DIRECTIONAL);
};

void DirectionalLight::SetDirection(const vec3& dir)
{
	m_LightData.m[0][0] = dir.x;
	m_LightData.m[0][1] = dir.y;
	m_LightData.m[0][2] = dir.z;
}

void DirectionalLight::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

TRE_NS_END