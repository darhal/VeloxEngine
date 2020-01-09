#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/DataStructure/String/String.hpp>

#include <Renderer/Common/Common.hpp>
#include <Renderer/Materials/Techniques/Technique.hpp>
#include <Renderer/Backend/RenderState/RenderState.hpp>

TRE_NS_START

class Material
{
public:
	Material(const AbstractMaterial& abstract_material, ShaderID shader_id = 0);

	FORCEINLINE Material();

	FORCEINLINE explicit Material(const String& m_Name);

	FORCEINLINE Material(Material&& other);

	FORCEINLINE Technique& GetTechnique();

	FORCEINLINE const Technique& GetTechnique() const;

    FORCEINLINE const RenderState& GetRenderStates() const;

    FORCEINLINE RenderState& GetRenderStates();

	FORCEINLINE const String& GetName() const;

	void GenerateShader();

	void Setup(const AbstractMaterial& abstract_material, ShaderID shader_id = 0);

	void Init(const AbstractMaterial& abstract_material, ShaderID shader_id = 0);

	NULL_COPY_AND_ASSIGN(Material);

private:
	Technique m_Technique;
	RenderState m_StateGroup;
	String m_Name;

	friend class MaterialLoader;
	friend class IPrimitiveMesh;
};

FORCEINLINE Material::Material(const String& name) :m_Technique(), m_StateGroup(), m_Name(name)
{
}

FORCEINLINE Material::Material() : m_Technique(), m_StateGroup(),m_Name("DEFAULT")
{
}

FORCEINLINE Material::Material(Material&& other) : 
	m_Technique(std::move(other.m_Technique)), m_StateGroup(other.m_StateGroup), m_Name(std::move(other.m_Name))
{
}

FORCEINLINE Technique& Material::GetTechnique()
{
	return m_Technique;
}

FORCEINLINE const Technique& Material::GetTechnique() const
{
	return m_Technique;
}

FORCEINLINE const RenderState& Material::GetRenderStates() const
{
    return m_StateGroup;
}

FORCEINLINE RenderState& Material::GetRenderStates()
{
    return m_StateGroup;
}

FORCEINLINE const String& Material::GetName() const
{
	return m_Name;
}

TRE_NS_END