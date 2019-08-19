#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <RenderAPI/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

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
	FORCEINLINE VBO() : m_ID(0), m_target(BufferTarget::UNKNOWN)
	{}

	~VBO();

	FORCEINLINE explicit VBO(BufferTarget::buffer_target_t target);

	FORCEINLINE uint32 Generate(BufferTarget::buffer_target_t target);

	template<typename T, ssize_t N>
	FORCEINLINE void FillData(T(&data)[N], BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW);

	FORCEINLINE void FillData(const void* data, ssize_t size, BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW);

	FORCEINLINE void SubFillData(const void* data, ssize_t offset, ssize_t length);

	FORCEINLINE void GetSubData(void* data, ssize_t offset, ssize_t length);

	FORCEINLINE const uint32 GetID() const;
	FORCEINLINE operator uint32() const;
	FORCEINLINE const int32 GetTarget() const;
	FORCEINLINE const TargetType::target_type_t GetBindingTarget() const { return (TargetType::target_type_t)m_target; }

	FORCEINLINE void Bind() const;
	FORCEINLINE void Use() const;

	FORCEINLINE void Unbind() const;
	FORCEINLINE void Unuse() const;

	FORCEINLINE void Clean();

	FORCEINLINE void Invalidate() { m_ID = 0; }

	explicit FORCEINLINE VBO(VBO&& other);
	FORCEINLINE VBO& operator=(VBO&& other);

	explicit FORCEINLINE VBO(const VBO& other) = delete;
	FORCEINLINE VBO& operator=(const VBO& other) = delete;

private:
	uint32 m_ID;
	BufferTarget::buffer_target_t m_target;
};

typedef VBO VertexBuffer;

#include "VBO.inl"

TRE_NS_END