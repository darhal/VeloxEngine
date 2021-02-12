#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "Buffer.hpp"

TRE_NS_START

namespace Renderer
{
	class RingBuffer : public Buffer
	{
	public:
        RingBuffer(RenderDevice& device, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem, uint32 unitSize, uint32 ringSize);

		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		uint32 GetUnitSize() const { return unitSize; }

		uint32 GetCurrentOffset() const { return bufferIndex * unitSize; }
	private:
		uint32		ringSize;
		uint32		unitSize;
		uint32		bufferIndex;

		friend class RenderBackend;
	};

	using RingBufferHandle = Handle<RingBuffer>;
}

TRE_NS_END
