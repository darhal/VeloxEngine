#include "Buffer.hpp"
#include <Renderer/Backend/SwapChain/SwapChain.hpp>

TRE_NS_START

Renderer::Buffer Renderer::Buffer::CreateBuffer(const Renderer::Internal::RenderDevice& renderDevice, VkDeviceSize size, const void* data,
	uint32 usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode, uint32 queueFamilyIndexCount, uint32* queueFamilyIndices
	)
{
	Buffer buffer;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size			= size;
	bufferInfo.usage		= usage;
	bufferInfo.sharingMode	= sharingMode;
	bufferInfo.flags		= 0;

	if (bufferInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
		bufferInfo.queueFamilyIndexCount = queueFamilyIndexCount;
		bufferInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	if (vkCreateBuffer(renderDevice.device, &bufferInfo, NULL, &buffer.apiBuffer) != VK_SUCCESS) {
		ASSERTF(true, "failed to create a buffer!");
	}

	return buffer;
}


void Renderer::Buffer::DestroyBuffer(const Internal::RenderDevice& renderDevice, Buffer& buffer)
{
	vkDestroyBuffer(renderDevice.device, buffer.apiBuffer, NULL);
	vkFreeMemory(renderDevice.device, buffer.bufferMemory.memory, nullptr);
}

uint32 Renderer::Buffer::FindMemoryType(const Internal::RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties)
{
	const VkPhysicalDeviceMemoryProperties& memProperties = renderDevice.memoryProperties;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	ASSERTF(true, "Failed to find suitable memory type!");
	return 0;
}

uint32 Renderer::Buffer::FindMemoryTypeIndex(const Internal::RenderDevice& renderDevice, uint32 typeFilter, MemoryUsage usage)
{
	VkMemoryPropertyFlags required = 0;
	VkMemoryPropertyFlags preferred = 0;

	switch (usage) {
	case MemoryUsage::USAGE_GPU_ONLY:
		required |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::USAGE_CPU_ONLY:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case MemoryUsage::USAGE_CPU_TO_GPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::USAGE_GPU_TO_CPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		ASSERTF(true, "Unknoiwn memory usage!");
	}

	return FindMemoryType(renderDevice, typeFilter, required);
}

void Renderer::Buffer::CopyBuffers(VkCommandBuffer cmdBuffer, uint32 count, Internal::TransferBufferInfo* transferBufferInfo)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);

	for (uint32 i = 0; i < count; i++) {
		const Internal::TransferBufferInfo& transferBuffer = transferBufferInfo[i];

		vkCmdCopyBuffer(cmdBuffer, transferBuffer.srcBuffer->apiBuffer, transferBuffer.dstBuffer->apiBuffer,
			(uint32)transferBuffer.copyRegions.Size(), transferBuffer.copyRegions.Data());
	}

	vkEndCommandBuffer(cmdBuffer);
}

void Renderer::Buffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
{
	ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

	void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + offset;
	//vkMapMemory(renderContext->renderDevice->device, bufferMemory.memory, bufferMemory.offset + offset, size, 0, &bufferData);
	memcpy(bufferData, data, size);
	//vkUnmapMemory(renderContext->renderDevice->device, bufferMemory.memory);
}

TRE_NS_END