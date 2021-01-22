#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/FileSystem/File/File.hpp>
#include <RenderAPI/Common.hpp>

TRE_NS_START

class Shader
{
private:
	ShaderType::shader_type_t type;
	uint32 ID;
public:
	Shader(const File& file, ShaderType::shader_type_t t);
	Shader(const String& code, ShaderType::shader_type_t t);
	~Shader() {};
	FORCEINLINE const uint32 GetID() const { return ID; }
	FORCEINLINE const ShaderType::shader_type_t GetType() const { return type; }
	friend class ShaderProgram;
};

TRE_NS_END