#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common.hpp>
#include <Core/Context/Extensions.hpp>
#include <Renderer/VertexBuffer/VBO.hpp>
#include <type_traits>
#include "VAODef.hpp"

TRE_NS_START

class VAO
{
public:
	VAO();
	~VAO();

	DECLARE_BIND_FUNCS

	template<typename T>
	void BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, uint32 stride, intptr offset);

	void BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, ssize_t stride, const void* offset);

	FORCEINLINE const int32 GetID() const { return m_ID; }
	FORCEINLINE operator uint32() const { return m_ID; }
	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return TargetType::VAO; }

	void Bind() const;
	void Use() const;

	void Unbind() const;
	void Unuse() const;
private:
	uint32 m_ID;
};

template<typename T>
void VAO::BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, uint32 stride, intptr offset)
{
	glBindVertexArray(m_ID);
	glBindBuffer(buffer.GetTarget(), buffer);
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, count, type, GL_FALSE, stride * sizeof(T), (const void*)(offset * sizeof(T)));
}

TRE_NS_END