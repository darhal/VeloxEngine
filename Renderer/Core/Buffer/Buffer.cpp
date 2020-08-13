#include "Buffer.hpp"
#include <Renderer/Core/SwapChain/SwapChain.hpp>

TRE_NS_START

Renderer::Buffer Renderer::CreateBuffer(const RenderDevice& renderDevice, VkDeviceSize size, const void* data, 
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

	if (vkCreateBuffer(renderDevice.device, &bufferInfo, NULL, &buffer.buffer) != VK_SUCCESS) {
		ASSERTF(true, "failed to create a buffer!");
	}

	AllocateMemory(renderDevice, buffer, properties);

	if (data) {
		void* bufferData;
		vkMapMemory(renderDevice.device, buffer.bufferMemory, 0, size, 0, &bufferData);
		memcpy(bufferData, data, size);
		vkUnmapMemory(renderDevice.device, buffer.bufferMemory);
	}

	return buffer;
}

Renderer::Buffer Renderer::CreateStaginBuffer(const RenderDevice& renderDevice, VkDeviceSize size, const void* data)
{
	return CreateBuffer(renderDevice, size, data, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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

	if (vkAllocateMemory(renderDevice.device, &allocInfo, NULL, &buffer.bufferMemory) != VK_SUCCESS) {
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
	return 0;
}

void Renderer::CopyBuffers(VkCommandBuffer cmdBuffer, uint32 count, TransferBufferInfo* transferBufferInfo)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);

	for (uint32 i = 0; i < count; i++) {
		const TransferBufferInfo& transferBuffer = transferBufferInfo[i];

		vkCmdCopyBuffer(cmdBuffer, transferBuffer.srcBuffer->buffer, transferBuffer.dstBuffer->buffer, 
			(uint32)transferBuffer.copyRegions.Size(), transferBuffer.copyRegions.Data());
	}

	vkEndCommandBuffer(cmdBuffer);
}

void Renderer::TransferBuffers(RenderContext& ctx, uint32 count, TransferBufferInfo* transferBufferInfo)
{
	VkCommandBuffer currentCmdBuff = TRE::Renderer::GetCurrentFrameResource(ctx).transferCommandBuffer;
	ctx.contextData.transferRequests = count;

	TRE::Renderer::CopyBuffers(currentCmdBuff, count, transferBufferInfo);
}

void Renderer::EditBuffer(const RenderDevice& renderDevice, Buffer& buffer, VkDeviceSize size, const void* data)
{
	if (data) {
		void* bufferData;
		vkMapMemory(renderDevice.device, buffer.bufferMemory, 0, size, 0, &bufferData);
		memcpy(bufferData, data, size);
		vkUnmapMemory(renderDevice.device, buffer.bufferMemory);
	}
}

TRE_NS_END