#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Synchronization/Semaphore/Semaphore.hpp>
#include <Renderer/Backend/CommandList/CommandPool.hpp>

TRE_NS_START

namespace Renderer
{
	class Image;
	class RenderDevice;
	class Blas;
	class Tlas;

	struct StagingBuffer
	{
        CommandBufferHandle blitCmdBuff;
        SemaphoreHandle     timelineSemaphore;
        VkBuffer            apiBuffer;
        VkDeviceSize        offset;
        uint8*              data;

        bool                submitted;
        bool                shouldRun; // Used for memory barriers and layout transitioning
        bool                isBlitting;
	};

	class RENDERER_API StagingManager
	{
	public:
        CONSTEXPR static uint32 NUM_STAGES = NUM_FRAMES;
        CONSTEXPR static uint32 NUM_CMDS   = NUM_FRAMES + 1;

        StagingManager(RenderDevice& renderDevice);

		~StagingManager();

		void Init();

		void Shutdown();

        void Stage(VkBuffer dstBuffer, const void* data, const DeviceSize size, const DeviceSize alignment = 1, DeviceSize offset = 0);

        void Stage(Image& dstImage, const void* data, const DeviceSize size, const DeviceSize alignment = 1);

		void* Stage(const DeviceSize size, const DeviceSize alignment, VkCommandBuffer& commandBuffer, VkBuffer& buffer, DeviceSize& bufferOffset);

		void ChangeImageLayout(Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess,
			bool needTopLevelBarrier = true);

		void GenerateMipmap(const Image& image);

		void ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, 
			VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

        bool Flush();

        void WaitPrevious();

		void Wait(StagingBuffer& stage);

        bool ResetPreviousStage();

        bool ResetCurrentStage();

        bool ResetNextStage();

        bool ResetStage(StagingBuffer& stage);

        bool ResetStage(uint32 i);

        FORCEINLINE StagingBuffer& GetPreviousStagingBuffer() { return stagingBuffers[(currentBuffer + (NUM_STAGES - 1)) % NUM_STAGES]; }
		FORCEINLINE StagingBuffer& GetCurrentStagingBuffer() { return stagingBuffers[currentBuffer]; }
        FORCEINLINE StagingBuffer& GetNextStagingBuffer() { return stagingBuffers[(currentBuffer + 1) % NUM_STAGES]; }
        FORCEINLINE StagingBuffer& GetStage(uint32 i = 0) { return stagingBuffers[i]; }

        FORCEINLINE const StagingBuffer& GetPreviousStagingBuffer() const { return stagingBuffers[(currentBuffer + (NUM_STAGES - 1)) % NUM_STAGES]; }
        FORCEINLINE const StagingBuffer& GetCurrentStagingBuffer() const { return stagingBuffers[currentBuffer]; }
        FORCEINLINE const StagingBuffer& GetNextStagingBuffer() const { return stagingBuffers[(currentBuffer + 1) % NUM_STAGES]; }
        FORCEINLINE const StagingBuffer& GetStage(uint32 i = 0) const { return stagingBuffers[i]; }

        FORCEINLINE CommandBufferHandle& GetCurrentCmd() { return transferCmdBuff[frameCounter]; };

        // FORCEINLINE CommandBufferHandle& GetCurrentCmd() { return command; };

        FORCEINLINE CommandBufferHandle& GetNextCmd() { return transferCmdBuff[(frameCounter + 1) % NUM_CMDS]; };

        FORCEINLINE void NextCmd() { frameCounter = (frameCounter + 1) % StagingManager::NUM_CMDS; }

        FORCEINLINE const SemaphoreHandle& GetTimelineSemaphore() const {
            //printf(" id: %d - ", (currentBuffer + (NUM_STAGES - 1)) % NUM_STAGES);
            return GetPreviousStagingBuffer().timelineSemaphore;
        }

        FORCEINLINE const SemaphoreHandle& GetTimelineSemaphore(uint32 i) const {
            // printf(" id: %d - ", i);
            return GetStage(i).timelineSemaphore;
        }

        FORCEINLINE CommandBufferHandle& GetBlitCmdBuffer() {
            return blitCommandPool ? blitCmdBuff[frameCounter] : this->GetCurrentCmd();
        }
	private:
		void PrepareFlush();

		void ChangeImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);
	private:
        RenderDevice& renderDevice;
        StagingBuffer       stagingBuffers[NUM_STAGES];
        CommandBufferHandle transferCmdBuff[NUM_CMDS];
        CommandBufferHandle blitCmdBuff[NUM_CMDS];
        CommandPoolHandle   commandPool;
        CommandPoolHandle   blitCommandPool;
		uint8*		        mappedData;
		VkDeviceMemory	    memory;
        uint32			    currentBuffer;
        uint32              frameCounter;
        bool                isBlitting;
		
        
		CONSTEXPR static uint32 MAX_UPLOAD_BUFFER_SIZE = 64 * 1024 * 1024;

        friend class RenderDevice;
	};
}


TRE_NS_END
