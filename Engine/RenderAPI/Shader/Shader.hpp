#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>

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

TRE_NS_START

class Shader
{
private:
	ShaderType::shader_type_t type;
	uint32 ID;
public:
	Shader(const char* path, ShaderType::shader_type_t t);
	~Shader() {};
	FORCEINLINE const uint32 GetID() const { return ID; }
	FORCEINLINE const ShaderType::shader_type_t GetType() const { return type; }
	friend class ShaderProgram;
};

TRE_NS_END