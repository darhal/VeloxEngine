#include "StagingManager.hpp"
#include <Renderer/Backend/Buffer/Buffer.hpp>

TRE_NS_START

namespace Renderer
{
	StagingManager::StagingManager(Internal::RenderContext* ctx) : ctx(ctx), currentBuffer(0)
	{
	}

	StagingManager::~StagingManager()
	{
	}

	void StagingManager::Shutdown()
	{
		VkDevice device = ctx->GetDevice();

		vkUnmapMemory(device, memory);

		for (uint32 i = 0; i < MAX_FRAMES; i++) {
			vkDestroyBuffer(device, stagingBuffers[i].apiBuffer, NULL);
			vkDestroyFence(device, stagingBuffers[i].transferFence, NULL);
		}

		vkDestroyCommandPool(device, commandPool, NULL);
		vkFreeMemory(device, memory, NULL);
	}

	void StagingManager::Init()
	{
		VkDevice device = ctx->GetDevice();

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = MAX_UPLOAD_BUFFER_SIZE;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		for (int i = 0; i < MAX_FRAMES; ++i) {
			stagingBuffers[i].offset = 0;

			vkCreateBuffer(device, &bufferCreateInfo, NULL, &stagingBuffers[i].apiBuffer);
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, stagingBuffers[0].apiBuffer, &memoryRequirements);

		const VkDeviceSize alignMod = memoryRequirements.size % memoryRequirements.alignment;
		const VkDeviceSize alignedSize = (alignMod == 0) ? memoryRequirements.size : (memoryRequirements.size + memoryRequirements.alignment - alignMod);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = alignedSize * MAX_FRAMES;
		memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryTypeIndex(*ctx->renderDevice, memoryRequirements.memoryTypeBits, MemoryUsage::USAGE_CPU_TO_GPU);

		vkAllocateMemory(device, &memoryAllocateInfo, NULL, &memory);
		vkMapMemory(device, memory, 0, alignedSize * MAX_FRAMES, 0, reinterpret_cast<void**>(&mappedData));

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = ctx->renderDevice->queueFamilyIndices.queueFamilies[TRANSFER];
		vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);

		{
			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = commandPool;
			commandBufferAllocateInfo.commandBufferCount = 1;

			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			for (int i = 0; i < MAX_FRAMES; i++) {
				vkBindBufferMemory(device, stagingBuffers[i].apiBuffer, memory, i * alignedSize);
				vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &stagingBuffers[i].transferCmdBuff);
				vkCreateFence(device, &fenceCreateInfo, NULL, &stagingBuffers[i].transferFence);
				vkBeginCommandBuffer(stagingBuffers[i].transferCmdBuff, &commandBufferBeginInfo);

				stagingBuffers[i].data = (uint8*)mappedData + (i * alignedSize);
			}
		}
	}

	void StagingManager::Stage(VkBuffer dstBuffer, const void* data, const DeviceSize size, const DeviceSize alignment, const DeviceSize offset)
	{
		if (size > MAX_UPLOAD_BUFFER_SIZE) {
			ASSERTF(true, "Can't allocate %d MB in GPU transfer buffer", (uint32)(size / (1024 * 1024)));
		}

		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		DeviceSize newOffset = stage->offset + size;
		DeviceSize padding = (alignment - (newOffset % alignment)) % alignment;
		stage->offset += padding;

		if ((stage->offset + size) >= (MAX_UPLOAD_BUFFER_SIZE) && !stage->submitted) {
			Flush();
		}

		stage = &stagingBuffers[currentBuffer];
		if (stage->submitted) {
			Wait(*stage);
		}

		uint8* stageBufferData = stage->data + stage->offset;
		memcpy(stageBufferData, data, size);
		VkBufferCopy bufferCopy{stage->offset, offset, size};
		vkCmdCopyBuffer(stage->transferCmdBuff, stage->apiBuffer, dstBuffer, 1, &bufferCopy);
		stage->offset += size;
	}

	void* StagingManager::Stage(const DeviceSize size, const DeviceSize alignment, VkCommandBuffer& commandBuffer, VkBuffer& buffer, DeviceSize& bufferOffset)
	{
		if (size > MAX_UPLOAD_BUFFER_SIZE) {
			ASSERTF(true, "Can't allocate %d MB in GPU transfer buffer", (uint32)(size / (1024 * 1024)));
		}

		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		DeviceSize offset = stage->offset + size;
		DeviceSize padding = (alignment - (offset % alignment)) % alignment;
		stage->offset += padding;

		if ((stage->offset + size) >= (MAX_UPLOAD_BUFFER_SIZE) && !stage->submitted) {
			Flush();
		}

		stage = &stagingBuffers[currentBuffer];
		if (stage->submitted) {
			Wait(*stage);
		}

		commandBuffer	= stage->transferCmdBuff;
		buffer			= stage->apiBuffer;
		bufferOffset	= stage->offset;

		uint8* data = stage->data + stage->offset;
		stage->offset += size;

		return data;
	}

	StagingBuffer* StagingManager::PrepareFlush()
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];

		if (stage.offset == 0 || stage.submitted) {
			return NULL;
		}

		VkCommandBuffer cmdBuff = stage.transferCmdBuff;

		if (!ctx->renderDevice->isTransferQueueSeprate) {
			VkMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
			vkCmdPipelineBarrier(
				cmdBuff,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				0, 1, &barrier, 0, NULL, 0, NULL);
		}

		vkEndCommandBuffer(cmdBuff);
		VkMappedMemoryRange memoryRange = {};
		memoryRange.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.memory	= memory;
		memoryRange.size	= VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(ctx->GetDevice(), 1, &memoryRange);
		return &stage;
	}

	void StagingManager::Prepare()
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];

		if (stage.submitted) {
			stage.offset = 0;
			stage.submitted = false;

			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			vkBeginCommandBuffer(stage.transferCmdBuff, &commandBufferBeginInfo);
		}
	}

	VkCommandBuffer StagingManager::Flush()
	{
		StagingBuffer* stage = this->PrepareFlush();

		if (!stage) {
			return VK_NULL_HANDLE;
		}

		VkCommandBuffer cmdBuff = stage->transferCmdBuff;

		if (ctx->renderDevice->isTransferQueueSeprate) {
			Internal::SwapChainData& swapChainData = ctx->swapChainData;

			ExecuteTransferMemory(ctx->renderDevice->queues[TRANSFER], cmdBuff,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence, ctx->GetDevice());
		} else {
			ExecuteTransferMemory(ctx->renderDevice->queues[GRAPHICS], cmdBuff,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence, ctx->GetDevice());
		}

		stage->submitted = true;
		currentBuffer = (currentBuffer + 1) % MAX_FRAMES;

		return cmdBuff;
	}

	void StagingManager::Wait(StagingBuffer& stage)
	{
		if (stage.submitted == false) {
			return;
		}

		vkWaitForFences(ctx->GetDevice(), 1, &stage.transferFence, VK_TRUE, UINT64_MAX);
		vkResetFences(ctx->GetDevice(), 1, &stage.transferFence);

		stage.offset	= 0;
		stage.submitted = false;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(stage.transferCmdBuff, &commandBufferBeginInfo);
	}

	void StagingManager::WaitCurrent()
	{
		uint32 prevBufferIndex = ((currentBuffer - 1) % MAX_FRAMES);

		if (prevBufferIndex < 0) {
			prevBufferIndex += MAX_FRAMES;
		}

		this->Wait(stagingBuffers[prevBufferIndex]);
	}

	void StagingManager::ExecuteTransferMemory(VkQueue queue, VkCommandBuffer cmdBuff, VkPipelineStageFlags waitStage, 
		VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkDevice device)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuff;


		if (waitSemaphore != VK_NULL_HANDLE) {
			submitInfo.waitSemaphoreCount	= 1;
			submitInfo.pWaitSemaphores		= &waitSemaphore;
			submitInfo.pWaitDstStageMask	= &waitStage;
		}

		if (signalSemaphore != VK_NULL_HANDLE) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores	= &signalSemaphore;
		}

		vkQueueSubmit(queue, 1, &submitInfo, fence);

		/*if (fence && device) {
			vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
			vkResetFences(device, 1, &fence);
		}*/
	}
}

TRE_NS_END