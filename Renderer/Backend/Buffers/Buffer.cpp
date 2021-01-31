#include "Buffer.hpp"
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>

TRE_NS_START

Renderer::Buffer::Buffer(RenderDevice& device, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem) : 
	device(device), apiBuffer(buffer), bufferInfo(info), bufferMemory(mem)
{
}

void Renderer::Buffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
{
	ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

	void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + offset;
	memcpy(bufferData, data, size);

	VkMappedMemoryRange range{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
	range.memory = bufferMemory.memory;
	range.offset = bufferMemory.offset + offset;
	range.size = size; //Utils::AlignUp(size, 256);
	//vkFlushMappedMemoryRanges(device.GetDevice(), 1, &range);
}

TRE_NS_END