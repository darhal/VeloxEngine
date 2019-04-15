#pragma once

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