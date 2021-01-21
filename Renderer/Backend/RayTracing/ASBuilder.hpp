#pragma once

#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;
	class Blas;
	class Tlas;
	class RenderBackend;

	class AsBuilder
	{
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
	public:
		AsBuilder(const RenderDevice& device);

		void Init();

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