#include "VAO.hpp"
#include <Core/Context/Extensions.hpp>
#include <Renderer/VertexBuffer/VBO.hpp>

TRE_NS_START

VAO::VAO()
{
	glGenVertexArrays(1, &m_ID);
}

void VAO::Bind() const
{
	glBindVertexArray(m_ID);
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &m_ID);
}

void VAO::BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, ssize_t stride, const void* offset)
{
	glBindVertexArray(m_ID);
	glBindBuffer(buffer.GetTarget(), buffer);
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, count, type, GL_FALSE, stride, offset);
}

TRE_NS_END