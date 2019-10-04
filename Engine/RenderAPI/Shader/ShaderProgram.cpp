#include "ShaderProgram.hpp"
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Logging.hpp>
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>

TRE_NS_START

ShaderProgram::ShaderProgram()
{
	m_ID = glCreateProgram();
}

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
{
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex.GetID());
	glAttachShader(m_ID, fragment.GetID());
	m_Shaders.EmplaceBack(vertex);
	m_Shaders.EmplaceBack(fragment);
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaderList) : m_Shaders(shaderList)
{
	m_ID = glCreateProgram();

	for (const Shader& shader : shaderList) {
		glAttachShader(m_ID, shader.GetID());
	}
}

void ShaderProgram::DestroyShaders()
{
	for(const Shader& shader : m_Shaders){
		glDeleteShader(shader.GetID());
	}
	m_Shaders.Clear();
}

void ShaderProgram::BindAttriute(uint32 index, const String& name)
{
	glBindAttribLocation(m_ID, index, name.Buffer());
}

void ShaderProgram::LinkProgram()
{
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	this->DestroyShaders();
}

void ShaderProgram::AddShader(const Shader& shader)
{
	glAttachShader(m_ID, shader.GetID());
	m_Shaders.EmplaceBack(shader);
}

uint8 ShaderProgram::AddSamplerSlot(const String& name)
{
	Uniform& uniform = this->AddUniform(name);
	return this->AddSamplerSlot(uniform);
}

uint8 ShaderProgram::AddSamplerSlot(Uniform& uniform)
{
	auto current_slot = m_Samplers.GetSize();
	this->SetInt(uniform.second, current_slot);
	m_Samplers.Emplace(&uniform.first, current_slot);
	return (uint8) current_slot;
}

uint8 ShaderProgram::GetSamplerSlot(Uniform& uniform) const
{
	return m_Samplers.GetPairPtr(&uniform.first)->second;
}

ShaderProgram::Uniform& ShaderProgram::AddUniform(const String& name)
{
	int32 slot = glGetUniformLocation(m_ID, name.Buffer());
	return m_Uniforms.Emplace(std::move(name), slot);
}

const ShaderProgram::Uniform& ShaderProgram::GetUniform(const String& name) const
{
	return *m_Uniforms.GetPairPtr(name);
}

uint32 ShaderProgram::GetUniformBlockIndex(const char* name) const
{
	return glGetUniformBlockIndex(m_ID, name);
}

void ShaderProgram::SetUniformBlockBinding(const char* name, uint32 index) const
{

	Call_GL(glUniformBlockBinding(m_ID, GetUniformBlockIndex(name), index));
}

void ShaderProgram::BindBufferBase(const VBO& vbo, uint32 bind_point) const
{
	Call_GL(glBindBufferBase(GL_UNIFORM_BUFFER, bind_point, vbo.GetID())); 
}

void ShaderProgram::BindBufferRange(const VBO& vbo, uint32 bind_point, uint32 offset, uint32 size)
{
	Call_GL(glBindBufferRange(GL_UNIFORM_BUFFER, bind_point, vbo.GetID(), offset, size));
}

void ShaderProgram::BindBufferBase(const UBO& ubo, uint32 bind_point) const
{
	Call_GL(glBindBufferBase(GL_UNIFORM_BUFFER, bind_point, ubo.GetVBO().GetID()));
}

// activate the shader
void ShaderProgram::Bind() const
{
	Call_GL(glUseProgram(m_ID));
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
	Call_GL(glUniform1i(m_Uniforms[name], (int)value));
}

void ShaderProgram::SetInt(const char* name, int value) const
{
	Call_GL(glUniform1i(m_Uniforms[name], value));
}

void ShaderProgram::SetFloat(const char* name, float value) const
{
	Call_GL(glUniform1f(m_Uniforms[name], value));
}

void ShaderProgram::SetVec2(const char* name, const vec2 &value) const
{
	Call_GL(glUniform2fv(m_Uniforms[name], 1, value.data));
}
void ShaderProgram::SetVec2(const char* name, float x, float y) const
{
	Call_GL(glUniform2f(m_Uniforms[name], x, y));
}

void ShaderProgram::SetVec3(const char* name, const vec3 &value) const
{
	Call_GL(glUniform3fv(m_Uniforms[name], 1, value.data));
}
void ShaderProgram::SetVec3(const char* name, float x, float y, float z) const
{
	Call_GL(glUniform3f(m_Uniforms[name], x, y, z));
}

void ShaderProgram::SetVec4(const char* name, const vec4 &value) const
{
	Call_GL(glUniform4fv(m_Uniforms[name], 1, value.data));
}
void ShaderProgram::SetVec4(const char* name, float x, float y, float z, float w)
{
	Call_GL(glUniform4f(m_Uniforms[name], x, y, z, w));
}

void ShaderProgram::SetMat2(const char* name, const Matrix<float, normal, 2, 2>& mat) const
{
	Call_GL(glUniformMatrix2fv(m_Uniforms[name], 1, GL_TRUE, mat.m[0]));
}

void ShaderProgram::SetMat3(const char* name, const Matrix<float, normal, 3, 3>& mat) const
{
	Call_GL(glUniformMatrix3fv(m_Uniforms[name], 1, GL_TRUE, mat.m[0]));
}

void ShaderProgram::SetMat4(const char* name, const Matrix<float, SIMD, 4, 4>& mat) const
{
	Call_GL(glUniformMatrix4fv(m_Uniforms[name], 1, GL_TRUE, mat.ptr()));
}

ShaderProgram::~ShaderProgram()
{
	if (!m_Shaders.IsEmpty()){
		this->DestroyShaders();
	}
	if (m_ID){
		glDeleteProgram(m_ID);
	}
}

void ShaderProgram::CheckCompileErrors(uint32 shader, const char* type)
{
#if defined(_DEBUG)
	int32 success;
	char infoLog[1024];

	if (strncmp(type, "PROGRAM", strlen(type)) == 0){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false){
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			///std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			Log::Write(Log::ERR, "(SHADER_COMPILATION_ERROR) in the '%s'.\n-->DEBUG INFO  : %s", type, infoLog);
		}
	}else{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);

		if (success == false){
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			///std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			Log::Write(Log::ERR, "(PROGRAM_LINKING_ERROR) in the '%s'.\n-->DEBUG INFO : %s", type, infoLog);
		}
	}
#endif
}

void ShaderProgram::SetBool(uint32 slot, bool value) const
{
	Call_GL(glUniform1i(slot, (int)value));
}

void ShaderProgram::SetInt(uint32 slot, int value) const
{
	Call_GL(glUniform1i(slot, value));
}

void ShaderProgram::SetFloat(uint32 slot, float value) const
{
	Call_GL(glUniform1f(slot, value));
}

void ShaderProgram::SetVec2(uint32 slot, const vec2 &value) const
{
	Call_GL(glUniform2fv(slot, 1, value.data));
}
void ShaderProgram::SetVec2(uint32 slot, float x, float y) const
{
	Call_GL(glUniform2f(slot, x, y));
}

void ShaderProgram::SetVec3(uint32 slot, const vec3 &value) const
{
	Call_GL(glUniform3fv(slot, 1, value.data));
}
void ShaderProgram::SetVec3(uint32 slot, float x, float y, float z) const
{
	Call_GL(glUniform3f(slot, x, y, z));
}

void ShaderProgram::SetVec4(uint32 slot, const vec4 &value) const
{
	Call_GL(glUniform4fv(slot, 1, value.data));
}
void ShaderProgram::SetVec4(uint32 slot, float x, float y, float z, float w)
{
	Call_GL(glUniform4f(slot, x, y, z, w));
}

void ShaderProgram::SetMat2(uint32 slot, const Matrix<float, normal, 2, 2>& mat) const
{
	Call_GL(glUniformMatrix2fv(slot, 1, GL_TRUE, mat.m[0]));
}

void ShaderProgram::SetMat3(uint32 slot, const Matrix<float, normal, 3, 3>& mat) const
{
	Call_GL(glUniformMatrix3fv(slot, 1, GL_TRUE, mat.m[0]));
}

void ShaderProgram::SetMat4(uint32 slot, const Matrix<float, SIMD, 4, 4>& mat) const
{
	Call_GL(glUniformMatrix4fv(slot, 1, GL_TRUE, mat.ptr()));
}

TRE_NS_END