#include "Material.hpp"
#include <RenderAPI/Texture/Texture.hpp>
#include "Renderer/Materials/AbstractMaterial/AbstractMaterial.hpp"
#include "Renderer/Materials/MaterialParametres/MaterialParamHelper.hpp"

TRE_NS_START

Material::Material(const AbstractMaterial& abstract_material, ShaderID shader_id) : 
	m_Technique(shader_id), 
	m_StateGroup(abstract_material.GetRenderStates()),
	m_Name(abstract_material.GetName())
{
	m_Technique.SetupAllUniforms(abstract_material);
}

void Material::GenerateShader()
{
}

void Material::Setup(const AbstractMaterial& abstract_material, ShaderID shader_id)
{
	// Setup the name :
	this->m_Name = abstract_material.GetName();

	// Intilize other variables :
	this->Init(abstract_material, shader_id);
}

void Material::Init(const AbstractMaterial& abstract_material, ShaderID shader_id)
{
	// Setup the technique :
	Technique& technique = this->GetTechnique();
	technique.SetShaderID(shader_id);
	technique.SetupAllUniforms(abstract_material);

	// setup rendering states... (Blending etc..)
	m_StateGroup = abstract_material.GetRenderStates();
}

TRE_NS_END