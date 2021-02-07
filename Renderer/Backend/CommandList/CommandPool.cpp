#include "CommandPool.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::CommandPool::CommandPool(RenderDevice* device, uint32 queueFamilyIndex) : 
	renderDevice(device), index(0), secondaryIndex(0)
{
	VkCommandPoolCreateInfo info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	info.queueFamilyIndex = queueFamilyIndex;

	vkCreateCommandPool(renderDevice->GetDevice(), &info, NULL, &pool);
}

Renderer::CommandPool::CommandPool() : renderDevice(NULL), pool(VK_NULL_HANDLE), index(0), secondaryIndex(0)
{
}

Renderer::CommandPool::~CommandPool()
{
    this->Destroy();
}

Renderer::CommandPool::CommandPool(CommandPool&& other) noexcept
{
	*this = std::move(other);
}

Renderer::CommandPool& Renderer::CommandPool::operator=(CommandPool&& other) noexcept
{
	if (this != &other) {
		renderDevice = other.renderDevice;

		if (pool != VK_NULL_HANDLE)
			vkDestroyCommandPool(renderDevice->GetDevice(), pool, nullptr);

		pool = VK_NULL_HANDLE;
		buffers.clear();
		std::swap(pool, other.pool);
		std::swap(buffers, other.buffers);
		index = other.index;
		other.index = 0;
	}
	return *this;
}

void Renderer::CommandPool::Reset()
{
	if (index > 0 || secondaryIndex > 0)
		vkResetCommandPool(renderDevice->GetDevice(), pool, 0);

	index = 0;
	secondaryIndex = 0;
}


VkCommandBuffer Renderer::CommandPool::RequestCommandBuffer()
{
	if (index < buffers.size()) {
		auto ret = buffers[index++];
		return ret;
	} else {
		VkCommandBuffer cmd;
		VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		info.commandPool = pool;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = 1;
		vkAllocateCommandBuffers(renderDevice->GetDevice(), &info, &cmd);
		buffers.push_back(cmd);
		index++;
		return cmd;
	}
}

VkCommandBuffer Renderer::CommandPool::RequestSecondaryCommandBuffer()
{
	if (secondaryIndex < secondaryBuffers.size()) {
		auto ret = secondaryBuffers[secondaryIndex++];
		return ret;
	} else {
		VkCommandBuffer cmd;
		VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		info.commandPool = pool;
		info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		info.commandBufferCount = 1;
		vkAllocateCommandBuffers(renderDevice->GetDevice(), &info, &cmd);
		secondaryBuffers.push_back(cmd);
		secondaryIndex++;
		return cmd;
	}
}

void Renderer::CommandPool::Destroy()
{
    if (!buffers.empty())
        vkFreeCommandBuffers(renderDevice->GetDevice(), pool, (uint32)buffers.size(), buffers.data());
    if (!secondaryBuffers.empty())
        vkFreeCommandBuffers(renderDevice->GetDevice(), pool, (uint32)secondaryBuffers.size(), secondaryBuffers.data());
    if (pool != VK_NULL_HANDLE)
        vkDestroyCommandPool(renderDevice->GetDevice(), pool, nullptr);

    pool = VK_NULL_HANDLE;
    buffers.clear();
    secondaryBuffers.clear();
}

TRE_NS_END

