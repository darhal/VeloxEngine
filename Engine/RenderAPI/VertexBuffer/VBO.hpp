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

struct VertexBufferSettings
{
	template<typename T>
	VertexBufferSettings(const T* data, uint32 count, BufferTarget::buffer_target_t target = BufferTarget::ARRAY_BUFFER, 
		BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW) :
		data(data), total_size(sizeof(T) * count), target(target), usage(usage)
	{}

	VertexBufferSettings(uint32 total_size, BufferTarget::buffer_target_t target = BufferTarget::ARRAY_BUFFER,
		BufferUsage::buffer_usage_t usage = BufferUsage::STATIC_DRAW) :
		data(NULL), total_size(total_size), target(target), usage(usage)
	{}

	const void* data;
	uint32 total_size;
	BufferTarget::buffer_target_t target;
	BufferUsage::buffer_usage_t usage;
};

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