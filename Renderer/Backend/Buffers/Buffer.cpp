#include "Buffer.hpp"
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>

TRE_NS_START

void Renderer::BufferDeleter::operator()(Buffer* buff)
{
    buff->device.GetObjectsPool().buffers.Free(buff);
}

Renderer::Buffer::Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem) :
    device(dev), bufferInfo(info), bufferMemory(mem), apiBuffer(buffer),
    ringSize(1), unitSize(info.size), bufferIndex(0)
{
}

Renderer::Buffer::Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info,
                         const MemoryView& mem, uint32 unitSize, uint32 ringSize) :
    device(dev), bufferInfo(info), bufferMemory(mem), apiBuffer(buffer),
    ringSize(ringSize), unitSize(unitSize), bufferIndex(0)
{
}

Renderer::Buffer::~Buffer()
{
    if (apiBuffer != VK_NULL_HANDLE) {
        device.DestroyBuffer(apiBuffer);
        device.FreeMemory(bufferMemory.allocKey);
        apiBuffer = VK_NULL_HANDLE;
    }
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

void Renderer::Buffer::WriteToRing(VkDeviceSize size, const void* data, VkDeviceSize offset)
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

TRE_NS_END
