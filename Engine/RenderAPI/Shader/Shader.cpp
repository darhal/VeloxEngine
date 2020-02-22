#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include <fstream>
#include <Core/Context/Extensions.hpp>

#if defined(COMPILER_MSVC)
	#pragma warning(disable:4996)
#endif

TRE_NS_START

Shader::Shader(const File& file, ShaderType::shader_type_t t) : type(t)
{
	String content = file.ReadAll();
	const char* buffer = content.Buffer();
	ID = glCreateShader((uint32)t);
	glShaderSource(ID, 1, &buffer, NULL);
	glCompileShader(ID);
	ShaderProgram::CheckCompileErrors(ID, ShaderType::ToString(t));
}

Shader::Shader(const String& code, ShaderType::shader_type_t t) : type(t)
{
	const char* code_str = code.Buffer();
	ID = glCreateShader((uint32)t);
	glShaderSource(ID, 1, &code_str, NULL);
	glCompileShader(ID);
	ShaderProgram::CheckCompileErrors(ID, ShaderType::ToString(t));
}

TRE_NS_END