#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Defines/DataStructure.hpp>
#include <RenderAPI/Texture/Texture.hpp>

TRE_NS_START

class Texture;

class Material
{
public:
	enum TextureMap {
		DIFFUSE  = 0,
		SPECULAR = 1,
		AMBIENT  = 2,
	};

	FORCEINLINE Material(const vec3& ambient, const vec3& diffuse, const vec3& specular);
	FORCEINLINE explicit Material(const String& m_Name);
	FORCEINLINE Material();

	void AddTexture(const TextureMap& type, const char* tex_path);
public:
	String m_Name;
	vec3 m_Ambient;
	vec3 m_Diffuse;
	vec3 m_Specular;
	float m_Shininess;
	Map<TextureMap, Texture> m_Textures;
	friend class MaterialLoader;
};


FORCEINLINE Material::Material(const vec3& ambient, const vec3& diffuse, const vec3& specular) :
	m_Ambient(ambient), m_Diffuse(diffuse), m_Specular(specular)
{
}

FORCEINLINE Material::Material(const String& name) : m_Name(name)
{
}

FORCEINLINE Material::Material() :
	m_Name("Default"), m_Ambient(vec3{ 1.f, 1.f, 1.f }),
	m_Diffuse(vec3(0.64f, 0.64f, 0.64f)), m_Specular(vec3(1.f, 1.f, 1.f))
{
}

TRE_NS_END