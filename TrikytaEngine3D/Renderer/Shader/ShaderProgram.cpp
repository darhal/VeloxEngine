#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Context/Extensions.hpp>

TRE_NS_START

ShaderProgram::ShaderProgram(uint32 vertex, uint32 fragment)
{
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

ShaderProgram::ShaderProgram(uint32 vertex, uint32 fragment, uint32 geometry)
{
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glAttachShader(ID, geometry);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaderList)
{
	ID = glCreateProgram();
	for (const Shader& shader : shaderList) {
		glAttachShader(ID, shader.GetID());
	}
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	for (const Shader& shader : shaderList) {
		glDeleteShader(shader.GetID());
	}
}


// activate the shader
void ShaderProgram::Use()
{
	glUseProgram(ID);
}

// utility uniform functions
void ShaderProgram::SetBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void ShaderProgram::SetInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void ShaderProgram::SetFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void ShaderProgram::SetVec2(const char* name, const vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name), 1, value.data);
}
void ShaderProgram::SetVec2(const char* name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void ShaderProgram::SetVec3(const char* name, const vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name), 1, value.data);
}
void ShaderProgram::SetVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void ShaderProgram::SetVec4(const char* name, const vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name), 1, value.data);
}
void ShaderProgram::SetVec4(const char* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void ShaderProgram::SetMat2(const char* name, const Matrix<float, normal, 2, 2>& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_TRUE, mat.m[0]);
}

void ShaderProgram::SetMat3(const char* name, const Matrix<float, normal, 3, 3>& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_TRUE, mat.m[0]);
}

void ShaderProgram::SetMat4(const char* name, const Matrix<float, SIMD, 4, 4>& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_TRUE, mat.ptr());
}

void ShaderProgram::CheckCompileErrors(uint32 shader, const char* type)
{
	int32 success;
	char infoLog[1024];
	if (type != "PROGRAM"){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success){
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			///std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			Log(LogType::ERR, "(SHADER_COMPILATION_ERROR) in the '%s'.\n-->DEBUG INFO  : %s", type, infoLog);
		}
	}else{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success){
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			///std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			Log(LogType::ERR, "(PROGRAM_LINKING_ERROR) in the '%s'.\n-->DEBUG INFO : %s", type, infoLog);
		}
	}
}

TRE_NS_END