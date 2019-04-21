#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Common.hpp>

TRE_NS_START

class Material
{
public:
	Material(const vec3& ambient, const vec3& diffuse, const vec3& specular);
	explicit Material(const String& m_Name);
	Material();
public:
	String m_Name;
	vec3 m_Ambient;
	vec3 m_Diffuse;
	vec3 m_Specular;

	friend class MaterialLoader;
};

TRE_NS_END