#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

namespace BufferTarget
{
	enum buffer_target_t {
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
		UNKNOWN = 0,
	};
}

namespace BufferUsage
{
	enum buffer_usage_t {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY
	};
}

class VBO
{
public:
	VBO();
	VBO(BufferTarget::buffer_target_t target);

	template<typename T, uint64 N>
	void FillData(T(&data)[N], BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW);

	void FillData(const void* data, ssize_t size, BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW);

	void SubFillData(const void* data, ssize_t offset, ssize_t length);

	void GetSubData(void* data, ssize_t offset, ssize_t length);

	void Bind() const;

	~VBO();
	FORCEINLINE const int32 GetID() const;
	FORCEINLINE operator uint32() const;
	FORCEINLINE const int32 GetTarget() const;
private:
	uint32 m_ID;
	BufferTarget::buffer_target_t m_target;
	ssize_t m_dataSize;
};

FORCEINLINE const int32 VBO::GetID() const 
{ 
	return m_ID; 
}

FORCEINLINE VBO::operator uint32() const 
{ 
	return m_ID; 
}

FORCEINLINE const int32 VBO::GetTarget() const
{
	return (int32)m_target;
}

template<typename T, uint64 N>
void VBO::FillData(T(&data)[N], BufferUsage::buffer_usage_t usage)
{
	ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to fill data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	glBindBuffer(m_target, m_ID);
	glBufferData(m_target, sizeof(data), data, usage);
}

TRE_NS_END