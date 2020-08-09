#include "Buffer.hpp"

TRE_NS_START

Renderer::Buffer Renderer::CreateBuffer(const RenderDevice& renderDevice, VkDeviceSize size, uint32 usage, VkMemoryPropertyFlags properties)
{
	Buffer buffer;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size			= size;
	bufferInfo.usage		= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags		= 0;

	if (vkCreateBuffer(renderDevice.device, &bufferInfo, NULL, &buffer.buffer) != VK_SUCCESS) {
		ASSERTF(true, "failed to create a buffer!");
	}

	AllocateMemory(renderDevice, buffer, properties);
	return buffer;
}

void Renderer::DestroyBuffer(const RenderDevice& renderDevice, Buffer& buffer)
{
	vkDestroyBuffer(renderDevice.device, buffer.buffer, NULL);
	vkFreeMemory(renderDevice.device, buffer.bufferMemory, nullptr);
}

void Renderer::AllocateMemory(const RenderDevice& renderDevice, Buffer& buffer, VkMemoryPropertyFlags properties)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(renderDevice.device, buffer.buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= FindMemoryType(renderDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(renderDevice.device, &allocInfo, nullptr, &buffer.bufferMemory) != VK_SUCCESS) {
		ASSERTF(true, "failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(renderDevice.device, buffer.buffer, buffer.bufferMemory, 0);
}

uint32 Renderer::FindMemoryType(const RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties)
{
	const VkPhysicalDeviceMemoryProperties& memProperties = renderDevice.memoryProperties;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	ASSERTF(true, "Failed to find suitable memory type!");
}

TRE_NS_END