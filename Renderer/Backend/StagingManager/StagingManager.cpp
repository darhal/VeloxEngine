#include "StagingManager.hpp"
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/CommandList/CommandList.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RayTracing/TLAS/TLAS.hpp>
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

namespace Renderer
{
	StagingManager::StagingManager(const RenderDevice& renderDevice) : 
		renderDevice(renderDevice), currentBuffer(0)
	{
	}

	StagingManager::~StagingManager()
	{
	}

	void StagingManager::Shutdown()
	{
		VkDevice device = renderDevice.GetDevice();

		vkUnmapMemory(device, memory);

		for (uint32 i = 0; i < NUM_FRAMES; i++) {
			vkDestroyBuffer(device, stagingBuffers[i].apiBuffer, NULL);
			vkDestroyFence(device, stagingBuffers[i].transferFence, NULL);
		}

		vkDestroyCommandPool(device, commandPool, NULL);
		vkFreeMemory(device, memory, NULL);
	}

	void StagingManager::Init()
	{
		VkDevice device = renderDevice.GetDevice();

		BufferCreateInfo info;
		info.domain = MemoryUsage::CPU_COHERENT;
		info.size = MAX_UPLOAD_BUFFER_SIZE;
		info.usage = BufferUsage::TRANSFER_SRC;

		for (int i = 0; i < NUM_FRAMES; ++i) {
			stagingBuffers[i].offset = 0;
			stagingBuffers[i].shouldRun = false;
			stagingBuffers[i].apiBuffer = renderDevice.CreateBuffer(info);
		}

		VkDeviceSize alignedSize = 0;
		memory = renderDevice.CreateBufferMemory(info, stagingBuffers[0].apiBuffer, &alignedSize, NUM_FRAMES);
		vkMapMemory(device, memory, 0, alignedSize * NUM_FRAMES, 0, reinterpret_cast<void**>(&mappedData));

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = renderDevice.GetQueueFamilyIndices().queueFamilies[TRANSFER];
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
			
			for (int i = 0; i < NUM_FRAMES; i++) {
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

	void StagingManager::Stage(Image& dstImage, const void* data, const DeviceSize size, const DeviceSize alignment)
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

		const ImageCreateInfo& info = dstImage.GetInfo();
		ChangeImageLayout(stage->transferCmdBuff, dstImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy imageCopy;
		imageCopy.bufferOffset = stage->offset;
		imageCopy.bufferRowLength = 0;
		imageCopy.bufferImageHeight = 0;
		imageCopy.imageSubresource = { FormatToAspectMask(info.format) , 0, 0, 1 };
		imageCopy.imageOffset = { 0, 0, 0 };
		imageCopy.imageExtent = { info.width, info.height, info.depth };

		vkCmdCopyBufferToImage(
			stage->transferCmdBuff,
			stage->apiBuffer,
			dstImage.apiImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &imageCopy
		);
		
		if (info.levels != 1) { // generate mipmaps:
			this->PrepareGenerateMipmapBarrier(dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

			this->GenerateMipmap(dstImage);

			// Manage layer trasnitioning :
			this->ImageBarrier(dstImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, info.layout,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, // TODO: change stages they are not specific!
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);
		} else {
			// Manage layer trasnitioning :
			ChangeImageLayout(stage->transferCmdBuff, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, info.layout);
		}

		stage->offset += size;
	}

	void StagingManager::ChangeImageLayout(Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];
		stage.shouldRun = true;

		// Put image pipline barrier for changing layout
		VkImageMemoryBarrier barrier{};
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout			= oldLayout;
		barrier.newLayout			= newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image				= image.GetApiObject();
		barrier.srcAccessMask		= ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: some doubt there
		barrier.dstAccessMask		= ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: same doubt
		barrier.subresourceRange.aspectMask		= FormatToAspectMask(image.GetInfo().format);
		barrier.subresourceRange.layerCount		= image.GetInfo().layers;
		barrier.subresourceRange.levelCount		= image.GetInfo().levels;

		VkPipelineStageFlags sourceStage	  = ImageUsageToPossibleStages(image.GetInfo().usage);
		VkPipelineStageFlags destinationStage = ImageUsageToPossibleStages(image.GetInfo().usage);

		vkCmdPipelineBarrier(
			stage.transferCmdBuff,
			sourceStage, destinationStage,
			0,
			0, NULL,
			0, NULL,
			1, &barrier
		);
	}

	void StagingManager::PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, bool needTopLevelBarrier)
	{
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		CommandBuffer cmd(NULL, stage->transferCmdBuff, CommandBuffer::Type::ASYNC_TRANSFER);

		cmd.PrepareGenerateMipmapBarrier(image, baseLevelLayout, srcStage, srcAccess, needTopLevelBarrier);
	}

	void StagingManager::GenerateMipmap(const Image& image)
	{
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		CommandBuffer cmd(NULL, stage->transferCmdBuff, CommandBuffer::Type::ASYNC_TRANSFER);

		cmd.GenerateMipmap(image);
	}

	void StagingManager::ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
		VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
	{
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		CommandBuffer cmd(NULL, stage->transferCmdBuff, CommandBuffer::Type::ASYNC_TRANSFER);

		cmd.ImageBarrier(image, oldLayout, newLayout, srcStage, srcAccess, dstStage, dstAccess);
	}

	StagingBuffer* StagingManager::PrepareFlush()
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];

		if (stage.shouldRun) {
			return &stage;
		}

		if (stage.offset == 0 || stage.submitted) {
			return NULL;
		}

		VkCommandBuffer cmdBuff = stage.transferCmdBuff;

		if (!renderDevice.IsTransferQueueSeprate()) {
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

		// vkEndCommandBuffer(cmdBuff);
		VkMappedMemoryRange memoryRange;
		memoryRange.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.pNext	= NULL;
		memoryRange.memory	= memory;
		memoryRange.offset	= currentBuffer * MAX_UPLOAD_BUFFER_SIZE;
		memoryRange.size	= MAX_UPLOAD_BUFFER_SIZE;

		vkFlushMappedMemoryRanges(renderDevice.GetDevice(), 1, &memoryRange);
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

		if (renderDevice.IsTransferQueueSeprate()) {
			renderDevice.SubmitCmdBuffer(TRANSFER, &cmdBuff, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence);
		} else {
			renderDevice.SubmitCmdBuffer(TRANSFER, &cmdBuff, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence);
		}

		stage->submitted = true;
		currentBuffer = (currentBuffer + 1) % NUM_FRAMES;
		return cmdBuff;
	}

	void StagingManager::Wait(StagingBuffer& stage)
	{
		if (stage.submitted == false) {
			return;
		}

		vkWaitForFences(renderDevice.GetDevice(), 1, &stage.transferFence, VK_TRUE, UINT64_MAX);
		vkResetFences(renderDevice.GetDevice(), 1, &stage.transferFence);

		stage.offset	= 0;
		stage.submitted = false;
		stage.shouldRun = false;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(stage.transferCmdBuff, &commandBufferBeginInfo);
	}

	void StagingManager::WaitCurrent()
	{
		uint32 prevBufferIndex = ((currentBuffer - 1) % NUM_FRAMES);

		if (prevBufferIndex < 0) {
			prevBufferIndex += NUM_FRAMES;
		}

		this->Wait(stagingBuffers[prevBufferIndex]);
	}

	void StagingManager::ChangeImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout			= oldLayout;
		barrier.newLayout			= newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image				= image.GetApiObject();
		barrier.subresourceRange.aspectMask		= FormatToAspectMask(image.GetInfo().format);
		barrier.subresourceRange.layerCount		= image.GetInfo().layers;
		barrier.subresourceRange.levelCount		= image.GetInfo().levels;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		//barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		//sourceStage			= ImageUsageToPossibleStages(image.GetInfo().usage);
		//destinationStage	= ImageUsageToPossibleStages(image.GetInfo().usage);

		vkCmdPipelineBarrier(
			cmd,
			sourceStage, destinationStage,
			0,
			0, NULL,
			0, NULL,
			1, &barrier
		);
	}
}

TRE_NS_END