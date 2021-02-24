#include "StagingManager.hpp"
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/CommandList/CommandList.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

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
		info.domain = MemoryUsage::CPU_COHERENT;
		info.size = MAX_UPLOAD_BUFFER_SIZE;
		info.usage = BufferUsage::TRANSFER_SRC;

        for (uint i = 0; i < NUM_STAGES; ++i) {
			stagingBuffers[i].offset = 0;
			stagingBuffers[i].shouldRun = false;
            stagingBuffers[i].apiBuffer = renderDevice.CreateBufferHelper(info);
		}

		VkDeviceSize alignedSize = 0;
        memory = renderDevice.CreateBufferMemory(info, stagingBuffers[0].apiBuffer, &alignedSize, NUM_STAGES);
        vkMapMemory(device, memory, 0, alignedSize * NUM_STAGES, 0, reinterpret_cast<void**>(&mappedData));
        commandPool = renderDevice.RequestCommandPool(QueueFamilyFlag::TRANSFER, CommandPool::CMD_BUFF_RESET);

        for (uint i = 0; i < NUM_STAGES; i++) {
            vkBindBufferMemory(device, stagingBuffers[i].apiBuffer, memory, i * alignedSize);
            // stagingBuffers[i].transferCmdBuff = commandPool->RequestCommandBuffer();
            stagingBuffers[i].timelineSemaphore = renderDevice.RequestTimelineSemaphore();
            stagingBuffers[i].data = (uint8*)mappedData + (i * alignedSize);
            // ////printf("cmd type: %d\n", stagingBuffers[i].transferCmdBuff->GetType());
        }

        for (uint i = 0; i < NUM_CMDS; i++){
            transferCmdBuff[i] = commandPool->RequestCommandBuffer();
        }
	}

	void StagingManager::Stage(VkBuffer dstBuffer, const void* data, const DeviceSize size, const DeviceSize alignment, const DeviceSize offset)
	{
		if (size > MAX_UPLOAD_BUFFER_SIZE) {
			ASSERTF(true, "Can't allocate %d MB in GPU transfer buffer", (uint32)(size / (1024 * 1024)));
		}

		StagingBuffer* stage = &stagingBuffers[currentBuffer];
		DeviceSize newOffset = stage->offset + size;
		DeviceSize padding = alignment ? (alignment - (newOffset % alignment)) % alignment : 0;
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
		DeviceSize padding = alignment ? (alignment - (offset % alignment)) % alignment : 0;
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
		barrier.srcAccessMask		= ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: some doubt there
		barrier.dstAccessMask		= ImageUsageToPossibleAccess(image.GetInfo().usage); // TODO: same doubt
		barrier.subresourceRange.aspectMask		= FormatToAspectMask(image.GetInfo().format);
		barrier.subresourceRange.layerCount		= image.GetInfo().layers;
		barrier.subresourceRange.levelCount		= image.GetInfo().levels;

		VkPipelineStageFlags sourceStage	  = ImageUsageToPossibleStages(image.GetInfo().usage);
		VkPipelineStageFlags destinationStage = ImageUsageToPossibleStages(image.GetInfo().usage);

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
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
        GetCurrentCmd()->PrepareGenerateMipmapBarrier(image, baseLevelLayout, srcStage, srcAccess, needTopLevelBarrier);
	}

	void StagingManager::GenerateMipmap(const Image& image)
	{
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
        GetCurrentCmd()->GenerateMipmap(image);
	}

	void StagingManager::ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
		VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
	{
		StagingBuffer* stage = &stagingBuffers[currentBuffer];
        GetCurrentCmd()->ImageBarrier(image, oldLayout, newLayout, srcStage, srcAccess, dstStage, dstAccess);
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

        VkCommandBuffer cmdBuff = GetCurrentCmd()->GetApiObject();

		if (!renderDevice.IsTransferQueueSeprate()) {
            VkMemoryBarrier barrier;
			barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            barrier.pNext = NULL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
			vkCmdPipelineBarrier(
				cmdBuff,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				0, 1, &barrier, 0, NULL, 0, NULL);
		}

		VkMappedMemoryRange memoryRange;
		memoryRange.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.pNext	= NULL;
		memoryRange.memory	= memory;
		memoryRange.offset	= currentBuffer * MAX_UPLOAD_BUFFER_SIZE;
		memoryRange.size	= MAX_UPLOAD_BUFFER_SIZE;

		vkFlushMappedMemoryRanges(renderDevice.GetDevice(), 1, &memoryRange);
		return &stage;
	}

    bool StagingManager::Flush()
	{
		StagingBuffer* stage = this->PrepareFlush();

		if (!stage) {
            return false;
		}

        //printf("FLUSH %d | CMD: %d\n", currentBuffer, frameCounter);
        //SemaphoreHandle wait = renderDevice.GetImageAcquiredSemaphore();
        SemaphoreHandle* signal[] = { &stage->timelineSemaphore };
        renderDevice.Submit(CommandBuffer::Type::ASYNC_TRANSFER, GetCurrentCmd(), NULL, 1, signal, 1);
        //renderDevice.AddWaitSemapore(CommandBuffer::ASYNC_TRANSFER, wait, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        renderDevice.FlushQueue(CommandBuffer::Type::ASYNC_TRANSFER);
		stage->submitted = true;
        currentBuffer = (currentBuffer + 1) % NUM_STAGES;
        return true;
	}

    void StagingManager::Wait(StagingBuffer& stage)
    {
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
        if (!stage.submitted) {
            //printf("ABORTED\n");
            return false;
        }

        //printf("(reset cmd buff:%d) COMPLETED\n", frameCounter);
        stage.offset	= 0;
        stage.submitted = false;
        stage.shouldRun = false;
        //transferCmdBuff[frameCounter]->ApiReset();
        transferCmdBuff[frameCounter]->Begin();
        return true;
    }

    bool StagingManager::ResetStage(uint32 i)
    {
        return this->ResetStage(this->GetStage(i));
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
