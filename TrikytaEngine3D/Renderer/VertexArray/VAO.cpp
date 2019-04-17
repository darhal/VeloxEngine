#include "VAO.hpp"
#include <Core/Context/Extensions.hpp>
#include <Renderer/VertexBuffer/VBO.hpp>
#include <Renderer/GlobalState/GLState.hpp>

TRE_NS_START

VAO::VAO()
{
	glGenVertexArrays(1, &m_ID);
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &m_ID);
}

void VAO::BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, ssize_t stride, const void* offset)
{
	this->Use(); //glBindVertexArray(m_ID);
	buffer.Use(); //glBindBuffer(buffer.GetTarget(), buffer);
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, count, type, GL_FALSE, stride, offset);
}

void VAO::Bind() const
{
	glBindVertexArray(m_ID);
}

void VAO::Use() const
{
	GLState::Bind(this);
}

void VAO::Unbind() const
{
	glBindVertexArray(0);
}

void VAO::Unuse() const
{
	GLState::Unbind(this);
}


TRE_NS_END