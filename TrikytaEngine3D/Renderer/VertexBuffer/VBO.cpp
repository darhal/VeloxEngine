#include "VBO.hpp"
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

VBO::VBO() : m_target(BufferTarget::UNKNOWN), m_dataSize(1)
{
	glGenBuffers(1, &m_ID);
}

VBO::VBO(BufferTarget::buffer_target_t target) : m_target(target), m_dataSize(1)
{
	glGenBuffers(1, &m_ID);
}

VBO::~VBO()
{
	glDeleteBuffers(1, &m_ID);
}

void VBO::FillData(const void* data, ssize_t size, BufferUsage::buffer_usage_t usage)
{
	ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to fill data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	glBindBuffer(m_target, m_ID);
	glBufferData(m_target, size, data, usage);
}

void VBO::SubFillData(const void* data, ssize_t offset, ssize_t length)
{
	ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to fill sub data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	glBindBuffer(m_target, m_ID);
	glBufferSubData(m_target, offset, length, data);
}

void VBO::GetSubData(void* data, ssize_t offset, ssize_t length)
{
	ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to get sub data of a vertex buffer object without setting the target (VBO ID = %d_n)", m_ID);
	glBindBuffer(m_target, m_ID);
	glGetBufferSubData(m_target, offset, length, data);
}

void VBO::Bind() const
{
	ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to bind a vertex buffer object without setting the target (VBO ID = %d_n)", m_ID);
	glBindBuffer(m_target, m_ID);
}

TRE_NS_END