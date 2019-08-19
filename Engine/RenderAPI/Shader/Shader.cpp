#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include <fstream>
#include <Core/Context/Extensions.hpp>

#if defined(COMPILER_MSVC)
	#pragma warning(disable:4996)
#endif

TRE_NS_START

Shader::Shader(const char* path, ShaderType::shader_type_t t) : type(t)
{
	FILE* file = fopen(path, "r");
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		const long size = ftell(file);
		char* buffer = new char[size];
		fseek(file, 0, SEEK_SET);
		char byte;
		uint64 index = 0;
		while ((byte = fgetc(file)) != EOF) {
			buffer[index] = byte;
			index++;
		}
		buffer[index] = '\0';
		fclose(file);
		ID = glCreateShader((uint32)t);
		glShaderSource(ID, 1, &buffer, NULL);
		glCompileShader(ID);
		ShaderProgram::CheckCompileErrors(ID, ShaderType::ToString(t));
		delete[] buffer;
	}
}

TRE_NS_END