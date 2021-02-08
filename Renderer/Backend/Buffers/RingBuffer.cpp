#include "RingBuffer.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

namespace Renderer
{
    RingBuffer::RingBuffer(RenderBackend& backend, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem, uint32 unitSize, uint32 ringSize)
        : Buffer(backend, buffer, info, mem), bufferIndex(0), ringSize(ringSize), unitSize(unitSize)
	{
	}

	void RingBuffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
	{
		ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

		bufferIndex = (bufferIndex + 1) % ringSize;
		void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + bufferIndex * unitSize + offset;
		memcpy(bufferData, data, size);

		VkMappedMemoryRange range{ VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
		range.memory = bufferMemory.memory;
		range.offset = bufferMemory.offset + bufferIndex * unitSize + offset;
		range.size = size;
		// vkFlushMappedMemoryRanges(device.GetDevice(), 1, &range);
	}
}

TRE_NS_END
