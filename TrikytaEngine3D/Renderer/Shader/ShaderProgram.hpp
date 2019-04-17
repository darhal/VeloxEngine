#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include <Core/Misc/Defines/DataStructure.hpp>
#include <Core/Context/GLDefines.hpp>
#include <initializer_list>
#include <Renderer/Common.hpp>

TRE_NS_START

namespace ShaderType{
	enum shader_type_t
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER
	};
	static FORCEINLINE const char* ToString(shader_type_t v) {
		static const char* shadertype2str[] = { "VERTEX SHADER", " FRAGMENT SHADER", "GEOMETRY SHADER" };
		switch (v) {
		case shader_type_t::VERTEX:
			return shadertype2str[0];
		case shader_type_t::FRAGMENT:
			return shadertype2str[1];
		case shader_type_t::GEOMETRY:
			return shadertype2str[2];
		default:
			return "Unknown";
		}
	}
}


class Shader;

class ShaderProgram
{
public:
	uint32 m_ID;
public:
	ShaderProgram(uint32 vertex, uint32 fragment);
	ShaderProgram(uint32 vertex, uint32 fragment, uint32 geometry);
	ShaderProgram(std::initializer_list<Shader> shaderList);

	// utility uniform functions
	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetVec2(const char* name, const vec2 &value) const;
	void SetVec2(const char* name, float x, float y) const;
	void SetVec3(const char* name, const vec3 &value) const;
	void SetVec3(const char* name, float x, float y, float z) const;
	void SetVec4(const char* name, const vec4 &value) const;
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat2(const char* name, const Matrix<float, normal, 2, 2>& mat) const;
	void SetMat3(const char* name, const Matrix<float, normal, 3, 3>& mat) const;
	void SetMat4(const char* name, const Matrix<float, SIMD, 4, 4>& mat) const;

	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return TargetType::target_type_t::SHADER; }
	FORCEINLINE const uint32 GetID() const { return m_ID; }
	void Use() const;
	void Unuse() const;

	// activate the shader
	void Bind() const;
	void Unbind() const;
private:
	// utility function for checking shader compilation/linking errors.
	static void CheckCompileErrors(uint32 shader, const char* type);
	friend class Shader;
};

TRE_NS_END