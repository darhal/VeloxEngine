#include "Buffer.hpp"
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::Buffer::Buffer(VkBuffer buffer, const BufferInfo& info, const MemoryView& mem) : 
	apiBuffer(buffer), bufferInfo(info), bufferMemory(mem)
{
}

void Renderer::Buffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
{
	ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

	void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + offset;
	memcpy(bufferData, data, size);
}

TRE_NS_END