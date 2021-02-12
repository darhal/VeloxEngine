#include "Buffer.hpp"
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>
#include "Renderer/Backend/Buffers/RingBuffer.hpp"

TRE_NS_START

void Renderer::BufferDeleter::operator()(Buffer* buff)
{
    // TODO: this is just ad-hoc solution the Buffer and RingBuffer class can be mixed togther in one class
    RingBuffer* rbuff = dynamic_cast<RingBuffer*>(buff);
    if (rbuff)
        buff->device.GetObjectsPool().buffers.Free(rbuff);
    else
        buff->device.GetObjectsPool().buffers.Free(buff);
}

Renderer::Buffer::Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem) :
    device(dev), apiBuffer(buffer), bufferInfo(info), bufferMemory(mem)
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

TRE_NS_END
