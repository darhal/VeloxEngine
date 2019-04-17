#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Context/Extensions.hpp>
#include <Renderer/GlobalState/GLState.hpp>

TRE_NS_START

ShaderProgram::ShaderProgram(uint32 vertex, uint32 fragment)
{
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

ShaderProgram::ShaderProgram(uint32 vertex, uint32 fragment, uint32 geometry)
{
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glAttachShader(m_ID, geometry);
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaderList)
{
	m_ID = glCreateProgram();
	for (const Shader& shader : shaderList) {
		glAttachShader(m_ID, shader.GetID());
	}
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	for (const Shader& shader : shaderList) {
		glDeleteShader(shader.GetID());
	}
}


// activate the shader
void ShaderProgram::Bind() const
{
	glUseProgram(m_ID);
}

void ShaderProgram::Use() const
{
	GLState::Bind(this);
}

void ShaderProgram::Unbind() const
{
	glUseProgram(0);
}

void ShaderProgram::Unuse() const
{
	GLState::Unbind(this);
}

// utility uniform functions
void ShaderProgram::SetBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name), (int)value);
}

void ShaderProgram::SetInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name), value);
}

void ShaderProgram::SetFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ID, name), value);
}

void ShaderProgram::SetVec2(const char* name, const vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(m_ID, name), 1, value.data);
}
void ShaderProgram::SetVec2(const char* name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_ID, name), x, y);
}

void ShaderProgram::SetVec3(const char* name, const vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name), 1, value.data);
}
void ShaderProgram::SetVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_ID, name), x, y, z);
}

void ShaderProgram::SetVec4(const char* name, const vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_ID, name), 1, value.data);
}
void ShaderProgram::SetVec4(const char* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(m_ID, name), x, y, z, w);
}

void ShaderProgram::SetMat2(const char* name, const Matrix<float, normal, 2, 2>& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ID, name), 1, GL_TRUE, mat.m[0]);
}

void ShaderProgram::SetMat3(const char* name, const Matrix<float, normal, 3, 3>& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, name), 1, GL_TRUE, mat.m[0]);
}

void ShaderProgram::SetMat4(const char* name, const Matrix<float, SIMD, 4, 4>& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_TRUE, mat.ptr());
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