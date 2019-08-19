#include "Technique.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"

TRE_NS_START

void Technique::UploadUnfiroms(const ShaderProgram& program) const
{
    for(const auto& uniform_data : m_MaterialParams.GetContainer<mat4>()){
        program.SetMat4(uniform_data.first, uniform_data.second);
    }

    for(const auto& uniform_data :  m_MaterialParams.GetContainer<vec3>()){
        program.SetVec3(uniform_data.first, uniform_data.second);
    }

    for(const auto& uniform_data : m_MaterialParams.GetContainer<float>()){
        program.SetFloat(uniform_data.first, uniform_data.second);
    }

    usize i = 0;
    for(const auto& uniform_data : m_MaterialParams.GetContainer<TextureID>()){
        Texture& active_tex = ResourcesManager::GetGRM().Get<Texture>(uniform_data.second);
        program.SetInt(uniform_data.first, i);
        glActiveTexture(GL_TEXTURE0 + i);
        active_tex.Bind();
        i++;
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
	this->SetupAllUniformsFromSameType<TextureID>(shader, abstract_material);
}

TRE_NS_END