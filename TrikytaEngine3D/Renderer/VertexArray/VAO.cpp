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

TRE_NS_END