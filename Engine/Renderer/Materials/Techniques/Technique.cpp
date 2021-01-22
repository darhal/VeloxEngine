#include "Technique.hpp"
#include "Renderer/Backend/ResourcesManager/ResourcesManager.hpp"
#include <RenderAPI/Texture/Texture.hpp>

TRE_NS_START

void Technique::UploadUnfiroms(const ShaderProgram& program) const
{
	m_MaterialParams.UploadUnfiroms(program);
}

void Technique::UploadUnfiroms() const
{
    // Query shader by ID from the reource manager :
    ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(m_ShaderID);

    this->UploadUnfiroms(shader);
}

void Technique::SetupAllUniforms(const AbstractMaterial& abstract_material)
{
    // Query shader by ID from the reource manager :
    ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(m_ShaderID);

    // for each built in type supported by GLSL set the uniforms one by one :
    this->SetupAllUniformsFromSameType<mat4>(shader, abstract_material);
	this->SetupAllUniformsFromSameType<vec3>(shader, abstract_material);
	this->SetupAllUniformsFromSameType<float>(shader, abstract_material);
	this->SetupAllSamplerSlots(shader, abstract_material);
}

void Technique::SetupAllSamplerSlots(const ShaderProgram& shader, const AbstractMaterial& abstract_material)
{
	for (const auto& var : abstract_material.GetParametres().GetContainer<TextureID>()) {
		ShaderProgram::Uniform& uniform = const_cast<ShaderProgram::Uniform&>(shader.GetUniform(var.first)); // Get the uniform
		m_MaterialParams.template AddParameter<TextureID>(shader.GetSamplerSlot(uniform), var.second); // Bind the sampler slot with the texture
	}
}

TRE_NS_END