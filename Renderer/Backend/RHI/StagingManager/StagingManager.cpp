#include "StagingManager.hpp"
#include <Renderer/Backend/RHI/Buffers/Buffer.hpp>
#include <Renderer/Backend/RHI/Images/Image.hpp>
#include <Renderer/Backend/RHI/CommandList/CommandList.hpp>
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>

TRE_NS_START

namespace Renderer
{
    StagingManager::StagingManager(RenderDevice& renderDevice) :
        renderDevice(renderDevice), currentBuffer(0), frameCounter(0)
	{
	}

	StagingManager::~StagingManager()
	{
	}

	void StagingManager::Shutdown()
	{
		VkDevice device = renderDevice.GetDevice();
        commandPool = CommandPoolHandle(NULL);
		blitCommandPool = CommandPoolHandle(NULL);
		vkUnmapMemory(device, memory);

        for (uint32 i = 0; i < NUM_STAGES; i++) {
			vkDestroyBuffer(device, stagingBuffers[i].apiBuffer, NULL);
            stagingBuffers[i].timelineSemaphore = SemaphoreHandle(NULL);
		}

		vkFreeMemory(device, memory, NULL);
	}

	void StagingManager::Init()
	{
		VkDevice device = renderDevice.GetDevice();

		BufferCreateInfo info;
		info.domain = MemoryDomain::CPU_COHERENT;
		info.size = MAX_UPLOAD_BUFFER_SIZE;
		info.usage = BufferUsage::TRANSFER_SRC;

        for (uint i = 0; i < NUM_STAGES; ++i) {
			stagingBuffers[i].apiBuffer = renderDevice.CreateBufferHelper(info);
			stagingBuffers[i].offset = 0;
			stagingBuffers[i].shouldRun = false;
			stagingBuffers[i].isBlitting = false;
		}

		VkDeviceSize alignedSize = 0;
        memory = renderDevice.CreateBufferMemory(info, stagingBuffers[0].apiBuffer, &alignedSize, NUM_STAGES);
        vkMapMemory(device, memory, 0, alignedSize * NUM_STAGES, 0, reinterpret_cast<void**>(&mappedData));
		// TODO: figure out how to fix this thing blit requires graphic while we should use async transfer
		if (renderDevice.IsTransferQueueSeprate())
			blitCommandPool = renderDevice.RequestCommandPool(QueueFamilyFlag::GRAPHICS, CommandPool::CMD_BUFF_RESET);
        commandPool = renderDevice.RequestCommandPool(QueueFamilyFlag::TRANSFER, CommandPool::CMD_BUFF_RESET);
		

		for (uint i = 0; i < NUM_CMDS; i++) {
			if (blitCommandPool)
				blitCmdBuff[i] = blitCommandPool->RequestCommandBuffer();

			transferCmdBuff[i] = commandPool->RequestCommandBuffer();
		}

        for (uint i = 0; i < NUM_STAGES; i++) {
            vkBindBufferMemory(device, stagingBuffers[i].apiBuffer, memory, i * alignedSize);
            // stagingBuffers[i].transferCmdBuff = commandPool->RequestCommandBuffer();
            stagingBuffers[i].timelineSemaphore = renderDevice.RequestTimelineSemaphore();
            stagingBuffers[i].data = (uint8*)mappedData + (i * alignedSize);
			stagingBuffers[i].blitCmdBuff = blitCommandPool ? this->GetBlitCmdBuffer() : CommandBufferHandle();
            // printf("cmd type: %d\n", stagingBuffers[i].transferCmdBuff->GetType());
        }
	}

    void StagingManager::PrepareFlush()
    {
        if (!blitCommandPool) {
			auto& cmdBuff = GetCurrentCmd();
			cmdBuff->FullBarrier();
        }

        /*VkMappedMemoryRange memoryRange;
        memoryRange.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        memoryRange.pNext	= NULL;
        memoryRange.memory	= memory;
        memoryRange.offset	= currentBuffer * MAX_UPLOAD_BUFFER_SIZE;
        memoryRange.size	= MAX_UPLOAD_BUFFER_SIZE;

        vkFlushMappedMemoryRanges(renderDevice.GetDevice(), 1, &memoryRange);*/
    }

    bool StagingManager::Flush()
    {
		StagingBuffer& stage = stagingBuffers[currentBuffer];

        if (!((stage.offset && !stage.submitted) || stage.shouldRun || stage.isBlitting)) {
			return false;
		}

		// this->PrepareFlush();

		if (stage.shouldRun || stage.offset) {
			SemaphoreHandle* signal[] = { &stage.timelineSemaphore };
			renderDevice.Submit(CommandBuffer::ASYNC_TRANSFER, GetCurrentCmd(), NULL, 1, signal, 1);
			renderDevice.FlushQueue(CommandBuffer::ASYNC_TRANSFER);
		}

		if (blitCommandPool && stage.isBlitting) {
			renderDevice.AddWaitTimelineSemapore(CommandBuffer::GENERIC, stage.timelineSemaphore, VK_PIPELINE_STAGE_TRANSFER_BIT);
			renderDevice.Submit(CommandBuffer::GENERIC, this->GetBlitCmdBuffer());
		}
		
        stage.submitted = true;
        currentBuffer = (currentBuffer + 1) % NUM_STAGES;
        return true;
    }

    void StagingManager::Wait(StagingBuffer& stage)
    {
		// We have no choice but to flush blit commands
		if (blitCommandPool && stage.isBlitting && !stage.blitCmdBuff->IsSubmitted()) {
			SemaphoreHandle* signal[] = { &stage.timelineSemaphore };
			renderDevice.AddSignalSemaphore(CommandBuffer::GENERIC, 1, signal, 1);
			renderDevice.FlushQueue(CommandBuffer::GENERIC);
		}

        stage.timelineSemaphore->Wait();
        this->ResetStage(stage);
    }

    void StagingManager::WaitPrevious()
    {
        this->Wait(this->GetPreviousStagingBuffer());
    }

    bool StagingManager::ResetPreviousStage()
    {
        //printf("Attempt to RESET %d ... ", (currentBuffer + (NUM_STAGES - 1)) % NUM_STAGES);
        return this->ResetStage(this->GetPreviousStagingBuffer());
    }

    bool StagingManager::ResetCurrentStage()
    {
        //printf("Attempt to RESET %d ... ", currentBuffer);
        return this->ResetStage(this->GetCurrentStagingBuffer());
    }

    bool StagingManager::ResetNextStage()
    {
        //printf("Attempt to RESET %d ... ", (currentBuffer + 1) % NUM_STAGES);
        return this->ResetStage(this->GetNextStagingBuffer());
    }

    bool StagingManager::ResetStage(StagingBuffer& stage)
    {
        if (!(stage.submitted ||  stage.shouldRun || stage.isBlitting)) {
            // printf("ABORTED\n");
            return false;
        }

        //printf("(reset cmd buff:%d) COMPLETED\n", frameCounter);
        stage.offset	= 0;
        stage.submitted = false;
        stage.shouldRun = false;
		stage.isBlitting = false;
		transferCmdBuff[frameCounter]->Begin();

		if (blitCommandPool) {
			stage.blitCmdBuff = this->GetBlitCmdBuffer();
			stage.blitCmdBuff->Begin();
		}
        
        return true;
    }

    bool StagingManager::ResetStage(uint32 i)
    {
        return this->ResetStage(this->GetStage(i));
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
            printf("FORCED TO FLUSH: %d\n", currentBuffer);
            this->Flush();
		}

		stage = &stagingBuffers[currentBuffer];
        if (stage->submitted) {
            printf("FORCED TO WAIT %d ... ", currentBuffer);
            this->Wait(*stage);
		}

        //printf("Writing to: %d | using cmd buff: %d\n", currentBuffer, frameCounter);
		uint8* stageBufferData = stage->data + stage->offset;
		memcpy(stageBufferData, data, size);
		VkBufferCopy bufferCopy{stage->offset, offset, size};
        vkCmdCopyBuffer(GetCurrentCmd()->GetApiObject(), stage->apiBuffer, dstBuffer, 1, &bufferCopy);
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
            this->Flush();
		}

		stage = &stagingBuffers[currentBuffer];
        if (stage->submitted) {
            this->Wait(*stage);
		}

        commandBuffer	= GetCurrentCmd()->GetApiObject();
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
		DeviceSize padding = alignment? (alignment - (newOffset % alignment)) % alignment : 0;
		stage->offset += padding;

		if ((stage->offset + size) >= (MAX_UPLOAD_BUFFER_SIZE) && !stage->submitted) {
            this->Flush();
		}

		stage = &stagingBuffers[currentBuffer];
        if (stage->submitted) {
            this->Wait(*stage);
		}

		uint8* stageBufferData = stage->data + stage->offset;
		memcpy(stageBufferData, data, size);

		const ImageCreateInfo& info = dstImage.GetInfo();
        ChangeImageLayout(GetCurrentCmd()->GetApiObject(), dstImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy imageCopy;
		imageCopy.bufferOffset = stage->offset;
		imageCopy.bufferRowLength = 0;
		imageCopy.bufferImageHeight = 0;
		imageCopy.imageSubresource = { FormatToAspectMask(info.format) , 0, 0, 1 };
		imageCopy.imageOffset = { 0, 0, 0 };
		imageCopy.imageExtent = { info.width, info.height, info.depth };

		vkCmdCopyBufferToImage(
            GetCurrentCmd()->GetApiObject(),
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
            ChangeImageLayout(GetCurrentCmd()->GetApiObject(), dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, info.layout);
		}

		stage->offset += size;
	}

	void StagingManager::ChangeImageLayout(Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];
		stage.shouldRun = true;

		// Put image pipline barrier for changing layout
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout			= oldLayout;
		barrier.newLayout			= newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image				= image.GetApiObject();
		barrier.srcAccessMask		= 0; // ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: some doubt there
		barrier.dstAccessMask		= 0; // ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: same doubt
		barrier.subresourceRange.aspectMask = FormatToAspectMask(image.GetInfo().format);
		barrier.subresourceRange.layerCount	= image.GetInfo().layers;
		barrier.subresourceRange.levelCount	= image.GetInfo().levels;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		/*if (renderDevice.IsTransferQueueSeprate()) {
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else {
			sourceStage = ImageUsageToPossibleStages(image.GetInfo().usage);
			destinationStage = ImageUsageToPossibleStages(image.GetInfo().usage);
		}*/

		vkCmdPipelineBarrier(
            GetCurrentCmd()->GetApiObject(),
			sourceStage, destinationStage,
			0,
			0, NULL,
			0, NULL,
			1, &barrier
		);
	}

	void StagingManager::PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, bool needTopLevelBarrier)
	{
		//StagingBuffer* stage = &stagingBuffers[currentBuffer];
        this->GetBlitCmdBuffer()->PrepareGenerateMipmapBarrier(image, baseLevelLayout, srcStage, srcAccess, needTopLevelBarrier);
	}

	void StagingManager::GenerateMipmap(const Image& image)
	{
		StagingBuffer& stage = stagingBuffers[currentBuffer];
		stage.isBlitting = true;

		this->GetBlitCmdBuffer()->GenerateMipmap(image);
	}

	void StagingManager::ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
		VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
	{
		// StagingBuffer* stage = &stagingBuffers[currentBuffer];
		this->GetBlitCmdBuffer()->ImageBarrier(image, oldLayout, newLayout, srcStage, srcAccess, dstStage, dstAccess);
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
        //destinationStage      = ImageUsageToPossibleStages(image.GetInfo().usage);

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
