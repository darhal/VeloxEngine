#pragma once

#include "Shader.hpp"
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <initializer_list>
#include <RenderAPI/Common.hpp>
#include <RenderEngine/Renderer/Backend/UBO/UBO.hpp> // Probably should be moved to RenderAPI

TRE_NS_START

class Shader;

class ShaderProgram
{
public:
	typedef HashMap<String, int32, PROBING>::HashNode Uniform;

public:
	uint32 m_ID;
	Vector<Shader> m_Shaders;
	HashMap<String, int32, PROBING> m_Uniforms;
	HashMap<String*, uint8, PROBING> m_Samplers;

public:
	ShaderProgram();
	ShaderProgram(const Shader& vertex, const Shader& fragment);
	ShaderProgram(std::initializer_list<Shader> shaderList);

	void DestroyShaders();

	void BindAttriute(uint32 index, const String& name);

	void AddShader(const Shader& shader);

	void LinkProgram();

	Uniform& AddUniform(const String& name);

	uint8 AddSamplerSlot(Uniform& uniform);

	uint8 AddSamplerSlot(const String& name);

	uint8 GetSamplerSlot(Uniform& uniform) const;

	const Uniform& GetUniform(const String& name) const;

	uint32 GetUniformBlockIndex(const char* name) const;

	void SetUniformBlockBinding(const char* name, uint32 index) const;

	void BindBufferBase(const VBO& vbo, uint32 bind_point) const;

	void BindBufferRange(const VBO& vbo, uint32 bind_point, uint32 offset, uint32 size);

	void BindBufferBase(const UBO& ubo, uint32 bind_point) const;

	~ShaderProgram();

	// utility uniform functions
	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetVec2(const char* name, const vec2& value) const;
	void SetVec2(const char* name, float x, float y) const;
	void SetVec3(const char* name, const vec3& value) const;
	void SetVec3(const char* name, float x, float y, float z) const;
	void SetVec4(const char* name, const vec4& value) const;
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat2(const char* name, const Matrix<float, normal, 2, 2>& mat) const;
	void SetMat3(const char* name, const Matrix<float, normal, 3, 3>& mat) const;
	void SetMat4(const char* name, const Matrix<float, SIMD, 4, 4>& mat) const;

	void SetBool(uint32 slot, bool value) const;
	void SetInt(uint32 slot, int value) const;
	void SetFloat(uint32 slot, float value) const;
	void SetVec2(uint32 slot, const vec2& value) const;
	void SetVec2(uint32 slot, float x, float y) const;
	void SetVec3(uint32 slot, const vec3& value) const;
	void SetVec3(uint32 slot, float x, float y, float z) const;
	void SetVec4(uint32 slot, const vec4& value) const;
	void SetVec4(uint32 slot, float x, float y, float z, float w);
	void SetMat2(uint32 slot, const Matrix<float, normal, 2, 2>& mat) const;
	void SetMat3(uint32 slot, const Matrix<float, normal, 3, 3>& mat) const;
	void SetMat4(uint32 slot, const Matrix<float, SIMD, 4, 4>& mat) const;

	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return TargetType::target_type_t::SHADER; }
	FORCEINLINE const uint32 GetID() const { return m_ID; }

	void Use() const;
	void Unuse() const;

	// activate the shader
	void Bind() const;
	void Unbind() const;

	ShaderProgram(const ShaderProgram& other) = delete;
	ShaderProgram& operator=(const ShaderProgram& other) = delete;

	explicit FORCEINLINE ShaderProgram(ShaderProgram&& other);
	FORCEINLINE ShaderProgram& operator=(ShaderProgram&& other);

private:
	// utility function for checking shader compilation/linking errors.
	static void CheckCompileErrors(uint32 shader, const char* type);
	friend class Shader;
};

FORCEINLINE ShaderProgram::ShaderProgram(ShaderProgram&& other) :
	m_ID(other.m_ID), m_Shaders(std::move(other.m_Shaders))
{
	other.m_ID = 0;
}

FORCEINLINE ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
	m_ID = other.m_ID;
	m_Shaders = std::move(other.m_Shaders);
	other.m_ID = 0;
	return *this;
}

TRE_NS_END