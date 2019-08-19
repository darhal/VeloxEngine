#include "VAO.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

TRE_NS_START

void VAO::BindAttribute(const uint32 attribute, const VBO& buffer, DataType::data_type_t type, uint32 count, int32 stride, const void* offset)
{
	this->Use(); //glBindVertexArray(m_ID);
	buffer.Use(); //glBindBuffer(buffer.GetTarget(), buffer);
	Call_GL(glEnableVertexAttribArray(attribute));
	Call_GL(glVertexAttribPointer(attribute, count, type, GL_FALSE, stride, offset));
}


TRE_NS_END