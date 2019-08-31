#include "DirectionalLight.hpp"

TRE_NS_START

DirectionalLight::DirectionalLight() 
{
	SetType(ILight::DIRECTIONAL);
};

void DirectionalLight::SetPosition(const vec3& pos)
{
	m_LightData.m[0][0] = pos.x;
	m_LightData.m[0][1] = pos.y;
	m_LightData.m[0][2] = pos.z;
}

void DirectionalLight::SetLightColor(const vec3& color)
{
	m_LightData.m[1][0] = color.x;
	m_LightData.m[1][1] = color.y;
	m_LightData.m[1][2] = color.z;
}

TRE_NS_END