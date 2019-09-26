#include "Technique.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"

TRE_NS_START

void Technique::UploadUnfiroms(const ShaderProgram& program) const
{
    for(const auto& uniform_data : m_MaterialParams.GetContainer<mat4>()) {
        program.SetMat4(uniform_data.first, uniform_data.second);
    }

    for(const auto& uniform_data :  m_MaterialParams.GetContainer<vec3>()) {
        program.SetVec3(uniform_data.first, uniform_data.second);
    }

    for(const auto& uniform_data : m_MaterialParams.GetContainer<float>()) {
        program.SetFloat(uniform_data.first, uniform_data.second);
    }

    for(const auto& uniform_data : m_MaterialParams.GetContainer<TextureID>()) {
        Texture& active_tex = ResourcesManager::GetGRM().Get<Texture>(uniform_data.second);
        glActiveTexture(GL_TEXTURE0 + uniform_data.first);
        active_tex.Bind();
    }
}

void Technique::UploadUnfiroms() const
{
    // Query shader by ID from the reource manager :
    ShaderProgram& shader = ResourcesManager::GetGRM().Get<ShaderProgram>(m_ShaderID);

    this->UploadUnfiroms(shader);
}

void Technique::SetupAllUniforms(const AbstractMaterial& abstract_material)
{
    // Query shader by ID from the reource manager :
    ShaderProgram& shader = ResourcesManager::GetGRM().Get<ShaderProgram>(m_ShaderID);

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