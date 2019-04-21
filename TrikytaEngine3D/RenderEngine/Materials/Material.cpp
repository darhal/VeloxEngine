#include "Material.hpp"

TRE_NS_START

Material::Material(const vec3& ambient, const vec3& diffuse, const vec3& specular) : 
	m_Ambient(ambient), m_Diffuse(diffuse), m_Specular(specular)
{
}

Material::Material(const String& name) : m_Name(name)
{
}

Material::Material() : 
	m_Name("Default"), m_Ambient(vec3{1.f, 1.f, 1.f}), 
	m_Diffuse(vec3(0.64f, 0.64f, 0.64f)), m_Specular(vec3(1.f, 1.f, 1.f))
{
}

TRE_NS_END