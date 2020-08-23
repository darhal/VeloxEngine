#include "RingBuffer.hpp"

TRE_NS_START

namespace Renderer
{
	void RingBuffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
	{
		ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

		void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + bufferIndex * unit_size + offset;
		memcpy(bufferData, data, size);
		bufferIndex = (bufferIndex + 1) % ring_size;
	}
}

TRE_NS_END