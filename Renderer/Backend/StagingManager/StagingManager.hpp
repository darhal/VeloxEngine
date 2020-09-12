#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class Image;

	struct StagingBuffer
	{
		VkCommandBuffer transferCmdBuff;
		VkBuffer		apiBuffer;
		VkFence			transferFence;
		VkDeviceSize	offset;
		uint8*			data;

		bool			submitted;
		bool			shouldRun; // Used for memory barriers and layout transitioning 
	};

	class RENDERER_API StagingManager
	{
	public:
		StagingManager(const Internal::RenderDevice* renderDevice);

		~StagingManager();

		void Init();

		void Shutdown();

		void Stage(VkBuffer dstBuffer, const void* data, const DeviceSize size, const DeviceSize alignment = 256, DeviceSize offset = 0);

		void Stage(Image& dstImage, const void* data, const DeviceSize size, const DeviceSize alignment = 256);

		void* Stage(const DeviceSize size, const DeviceSize alignment, VkCommandBuffer& commandBuffer, VkBuffer& buffer, DeviceSize& bufferOffset);

		void ChangeImageLayout(Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void Prepare();

		VkCommandBuffer Flush();

		void Wait(StagingBuffer& stage);

		void WaitCurrent();

		FORCEINLINE StagingBuffer& GetCurrentStagingBuffer() { return stagingBuffers[currentBuffer]; }
	private:
		StagingBuffer* PrepareFlush();

		void ExecuteTransferMemory(VkQueue queue, VkCommandBuffer cmdBuff, VkPipelineStageFlags waitStage,
			VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE);

		void ChangeImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);
	private:
		StagingBuffer   stagingBuffers[NUM_FRAMES];
		uint8*		    mappedData;
		VkDeviceMemory	memory;
		VkCommandPool	commandPool;
		uint32			currentBuffer;
		

		const Internal::RenderDevice* renderDevice;
		
		CONSTEXPR static uint32 MAX_UPLOAD_BUFFER_SIZE = 64 * 1024 * 1024;
	};
}


TRE_NS_END
