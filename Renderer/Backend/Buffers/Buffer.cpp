#include "Buffer.hpp"
#include <Renderer/Backend/SwapChain/SwapChain.hpp>

TRE_NS_START


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
	// VkMemoryPropertyFlags preferred = 0;

	switch (usage) {
	case MemoryUsage::GPU_ONLY:
		required |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		// preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::LINKED_GPU_CPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::CPU_ONLY:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		// preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::CPU_CACHED:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		// preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	case MemoryUsage::CPU_COHERENT:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		// preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		ASSERTF(true, "Unknown memory usage!");
	}

	return FindMemoryType(renderDevice, typeFilter, required);
}

void Renderer::Buffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
{
	ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

	void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + offset;
	memcpy(bufferData, data, size);
}

TRE_NS_END