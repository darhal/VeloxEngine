#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/Misc/Maths/Maths.hpp>

#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>

#include "RenderEngine/Materials/AbstractMaterial/AbstractMaterial.hpp"
#include <RenderEngine/Materials/MaterialParametres/MaterialParametres.hpp>

TRE_NS_START

class Technique
{
public:
    typedef MaterialParametres<uint8> MaterialParametresContainer;

public:
    FORCEINLINE Technique(ShaderID shaderID = 0);

    FORCEINLINE Technique(Technique&& other);

    FORCEINLINE void SetUniformMat4(const ShaderProgram::Uniform& uniform, const mat4& mat);

    FORCEINLINE void SetUniformVec3(const ShaderProgram::Uniform& uniform, const vec3& vec);

    FORCEINLINE void SetUniformFloat(const ShaderProgram::Uniform& uniform, float real);

    FORCEINLINE void SetUniformSampler(const ShaderProgram::Uniform& uniform, TextureID sampler);

    template<typename T, typename... Args>
    FORCEINLINE void SetUniform(const ShaderProgram::Uniform& uniform, Args&&... args);

    FORCEINLINE void SetShaderID(ShaderID shader_id);

    FORCEINLINE ShaderID GetShaderID() const;

    FORCEINLINE MaterialParametresContainer& GetMaterialParametresContainer();

    void UploadUnfiroms(const ShaderProgram& program) const;

    void UploadUnfiroms() const;

    template<typename UNIFORM_TYPE>
    void SetupAllUniformsFromSameType(const ShaderProgram& shader, const AbstractMaterial& abstract_material);

    void SetupAllUniforms(const AbstractMaterial& abstract_material);

private:
    MaterialParametresContainer m_MaterialParams;
    ShaderID m_ShaderID;

    friend class Material;
};

FORCEINLINE Technique::Technique(ShaderID shaderID) :
    m_MaterialParams(), 
    m_ShaderID(shaderID)
{
}

FORCEINLINE void Technique::SetUniformMat4(const ShaderProgram::Uniform& uniform, const mat4& mat)
{
    m_MaterialParams.template AddParameter<mat4>(uniform.value, mat);
}

FORCEINLINE void Technique::SetUniformVec3(const ShaderProgram::Uniform& uniform, const vec3& vec)
{
    m_MaterialParams.template AddParameter<vec3>(uniform.value, vec);
}

FORCEINLINE void Technique::SetUniformFloat(const ShaderProgram::Uniform& uniform, float real)
{
    m_MaterialParams.template AddParameter<float>(uniform.value, real);
}

FORCEINLINE void Technique::SetUniformSampler(const ShaderProgram::Uniform& uniform, TextureID sampler)
{
    m_MaterialParams.template AddParameter<TextureID>(uniform.value, sampler);
}

template<typename T, typename... Args>
FORCEINLINE void Technique::SetUniform(const ShaderProgram::Uniform& uniform, Args&&... args)
{
    m_MaterialParams.template AddParameter<T>(uniform.value, std::forward<Args>(args)...);
}

FORCEINLINE void Technique::SetShaderID(ShaderID shader_id)
{
    m_ShaderID = shader_id;
    m_MaterialParams.Clear();
}

FORCEINLINE ShaderID Technique::GetShaderID() const
{
    return m_ShaderID;
}

template<typename UNIFORM_TYPE>
void Technique::SetupAllUniformsFromSameType(const ShaderProgram& shader, const AbstractMaterial& abstract_material)
{
    for(const auto& var : abstract_material.GetParametres().GetContainer<UNIFORM_TYPE>()){
		this->template SetUniform<UNIFORM_TYPE>(shader.GetUniform(var.first), var.second);
	}
}

FORCEINLINE typename Technique::MaterialParametresContainer& Technique::GetMaterialParametresContainer()
{
    return m_MaterialParams;
}

FORCEINLINE Technique::Technique(Technique&& other) : 
    m_MaterialParams(std::move(other.m_MaterialParams)),
    m_ShaderID(other.m_ShaderID)
{
}

TRE_NS_END