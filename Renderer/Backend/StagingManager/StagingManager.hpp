#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class Image;
	class RenderDevice;
	class Blas;
	class Tlas;
	class RenderBackend;

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

	struct RtStaging
	{
		VkCommandPool cmdPool;
		VkBuffer scratchBuffer;
		VkDeviceAddress address;
		VkFence fence;

		VkBuffer stagingBuffer;
		VkDeviceSize offset;
		uint8* data;

		struct Batch
		{
			struct BatchInfo
			{
				VkCommandBuffer cmd;
				VkAccelerationStructureBuildGeometryInfoKHR buildInfo;
				std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;
				Blas* blasObject;
			};

			std::vector<BatchInfo> batchInfo;
			VkQueryPool queryPool;
			VkDeviceSize scratchSize;
		};
		
		// Compact Data:
		std::vector<VkDeviceSize> compactSizes;
		std::vector<const VkAccelerationStructureBuildRangeInfoKHR*> pBuildOffset;
		std::vector<VkAccelerationStructureKHR> cleanupAS;
		VkCommandBuffer compressionCommand;
		// 0: not compact, 1: compact
		Batch batch[2];

		// Bulding TLAS:
		struct TlasBuild
		{
			VkAccelerationStructureBuildGeometryInfoKHR buildInfo;
			Tlas* tlasObject;
		};
		std::vector<TlasBuild> tlasBuilds;

		bool submitted;
	};

	class RENDERER_API StagingManager
	{
	public:
		StagingManager(const RenderDevice& renderDevice);

		~StagingManager();

		void Init();

		void InitRT();

		void Shutdown();

		void Stage(VkBuffer dstBuffer, const void* data, const DeviceSize size, const DeviceSize alignment = 256, DeviceSize offset = 0);

		void Stage(Image& dstImage, const void* data, const DeviceSize size, const DeviceSize alignment = 256);

		void* Stage(const DeviceSize size, const DeviceSize alignment, VkCommandBuffer& commandBuffer, VkBuffer& buffer, DeviceSize& bufferOffset);

		void ChangeImageLayout(Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess,
			bool needTopLevelBarrier = true);

		void GenerateMipmap(const Image& image);

		void ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, 
			VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

		void Prepare();

		VkCommandBuffer Flush();

		void Wait(StagingBuffer& stage);

		void WaitCurrent();

		FORCEINLINE StagingBuffer& GetCurrentStagingBuffer() { return stagingBuffers[currentBuffer]; }

		// RT Functionality:
		void StageBlasBuilding(Blas* blas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo,
			const VkAccelerationStructureBuildRangeInfoKHR* ranges, uint32 rangesCount = 1, 
			uint32 flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);

		void StageTlasBuilding(Tlas* tlas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo, 
			VkBuildAccelerationStructureFlagsKHR flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);

		void BuildTlasBatch();

		void BuildBlasBatch(bool compact);

		void CompressBatch(RenderBackend& backend);

		void BuildBlasBatchs();

		void SyncAcclBuilding();

		void BuildAll(RenderBackend& backend);
	private:
		StagingBuffer* PrepareFlush();

		VkResult SubmitCommandBuffer(VkQueue queue, VkCommandBuffer* cmdBuff, uint32 cmdCount, VkPipelineStageFlags waitStage,
			VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE);

		void ChangeImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkCommandBuffer CreateCommandBuffer(VkCommandPool pool);
	private:
		StagingBuffer   stagingBuffers[NUM_FRAMES];
		uint8*		    mappedData;
		VkDeviceMemory	memory;
		VkCommandPool	commandPool;
		uint32			currentBuffer;
		
		// RT Functionality:
		RtStaging rtStaging[NUM_FRAMES];
		uint32 currentStaging;
		VkDeviceMemory scartchMemory;
		uint32 maxScratchSize;
		VkDeviceMemory stagingMemory;
		uint8* stagingMappedData;


		const RenderDevice& renderDevice;
		
		CONSTEXPR static uint32 MAX_UPLOAD_BUFFER_SIZE = 64 * 1024 * 1024;
	};
}


TRE_NS_END
