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
		,maxScratchSize(MAX_UPLOAD_BUFFER_SIZE), currentStaging(0)
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
		this->InitRT();

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

	// RT functionality:
	void StagingManager::InitRT()
	{
		BufferCreateInfo info;
		info.domain = MemoryUsage::GPU_ONLY;
		info.size = maxScratchSize;
		info.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::STORAGE_BUFFER;

		BufferCreateInfo stagingInfo;
		stagingInfo.domain = MemoryUsage::CPU_COHERENT;
		stagingInfo.size = MAX_UPLOAD_BUFFER_SIZE;
		stagingInfo.usage = BufferUsage::TRANSFER_SRC;

		for (int i = 0; i < NUM_FRAMES; i++) {
			rtStaging[i].scratchBuffer = renderDevice.CreateBuffer(info);
			rtStaging[i].address = NULL;
			rtStaging[i].submitted = false;
			rtStaging[i].stagingBuffer = renderDevice.CreateBuffer(stagingInfo);

			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			//TODO: we are doing RT here so what should we do maybe transfer ?
			commandPoolCreateInfo.queueFamilyIndex = renderDevice.GetQueueFamilyIndices().queueFamilies[COMPUTE];
			vkCreateCommandPool(renderDevice.GetDevice(), &commandPoolCreateInfo, NULL, &rtStaging[i].cmdPool);
		}

		VkDeviceSize stagingAlignedSize = 0;
		stagingMemory = renderDevice.CreateBufferMemory(stagingInfo, rtStaging[0].stagingBuffer, &stagingAlignedSize, NUM_FRAMES);
		vkMapMemory(renderDevice.GetDevice(), stagingMemory, 0, stagingAlignedSize * NUM_FRAMES, 0, reinterpret_cast<void**>(&stagingMappedData));

		VkDeviceSize alignedSize = 0;
		scartchMemory = renderDevice.CreateBufferMemory(info, rtStaging[0].scratchBuffer, &alignedSize, NUM_FRAMES);
		VkBufferDeviceAddressInfo bufferAdrInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
		
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		for (int i = 0; i < NUM_FRAMES; i++) {
			vkBindBufferMemory(renderDevice.GetDevice(), rtStaging[i].scratchBuffer, scartchMemory, i * alignedSize);

			bufferAdrInfo.buffer = rtStaging[i].scratchBuffer;
			rtStaging[i].address = vkGetBufferDeviceAddressKHR(renderDevice.GetDevice(), &bufferAdrInfo);
			vkCreateFence(renderDevice.GetDevice(), &fenceCreateInfo, NULL, &rtStaging[i].fence);

			// Staging buffer for RT:
			vkBindBufferMemory(renderDevice.GetDevice(), rtStaging[i].stagingBuffer, stagingMemory, i * stagingAlignedSize);
			rtStaging[i].data = (uint8*)stagingMappedData + (i * alignedSize);
			rtStaging[i].offset = 0;
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

	void StagingManager::BuildBlasBatchs()
	{
		// Start by building compact ones as they have dependcies later
		this->BuildBlasBatch(1);
		this->BuildBlasBatch(0);
	}

	void StagingManager::SyncAcclBuilding()
	{
		RtStaging* stage = &rtStaging[currentStaging];
		bool fenceSet = stage->submitted;

		if (!fenceSet)
			return;

		// wait till we get everything done to re-claim resources
		vkWaitForFences(renderDevice.GetDevice(), 1, &stage->fence, VK_TRUE, UINT64_MAX);
		vkResetFences(renderDevice.GetDevice(), 1, &stage->fence);
		stage->submitted = false;

		vkResetCommandPool(renderDevice.GetDevice(), stage->cmdPool, 0);

		for (uint32 i = 0; i < stage->batch[1].batchInfo.size(); i++) {
			vkDestroyAccelerationStructureKHR(renderDevice.GetDevice(), stage->cleanupAS[i], NULL);
		}

		for (uint32 i = 0; i < NUM_FRAMES; i++) {
			stage->batch[i].batchInfo.clear();
			stage->offset = 0;
		}

		// TRE_LOGI("Building and compression DONE.\n");
	}

	void StagingManager::StageTlasBuilding(Tlas* tlas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo,
		VkBuildAccelerationStructureFlagsKHR flags)
	{
		RtStaging* stage = &rtStaging[currentStaging];

		// wait till we get everything done to re-claim resources
		this->SyncAcclBuilding();

		stage->tlasBuilds.emplace_back();
		RtStaging::TlasBuild& build = stage->tlasBuilds.back();
		build.tlasObject = tlas;
		build.buildInfo = buildInfo;
		stage->tlasBuilds.emplace_back(build);
	}

	void StagingManager::BuildTlasBatch()
	{
		RtStaging* stage = &rtStaging[currentStaging];
		bool fenceSet = stage->submitted;
		size_t nbTlas = stage->tlasBuilds.size();
		auto cmdBuff = this->CreateCommandBuffer(stage->cmdPool);

		if (fenceSet) {
			printf("TLAS:Waiting for fence\n");
			vkWaitForFences(renderDevice.GetDevice(), 1, &stage->fence, VK_TRUE, UINT64_MAX);
			vkResetFences(renderDevice.GetDevice(), 1, &stage->fence);
			stage->submitted = false;

			vkResetCommandPool(renderDevice.GetDevice(), stage->cmdPool, 0);
		}

		std::vector<VkAccelerationStructureInstanceKHR> geometryInstances(512);
		
		for (size_t i = 0; i < nbTlas; i++) {
			auto& info = stage->tlasBuilds[i];
			auto& buildInfo = info.buildInfo;
			Tlas* tlas = info.tlasObject;
			bool update = false;

			geometryInstances.clear();
			geometryInstances.reserve(tlas->GetInfo().blasInstances.size());

			for (const BlasInstance& inst : tlas->GetInfo().blasInstances) {
				geometryInstances.emplace_back();
				VkAccelerationStructureInstanceKHR& acclInst = geometryInstances.back();
				acclInst.instanceCustomIndex = inst.instanceCustomId;
				acclInst.mask = inst.mask;
				acclInst.instanceShaderBindingTableRecordOffset = inst.hitGroupId;
				acclInst.flags = inst.flags;
				acclInst.accelerationStructureReference = inst.blas->GetAcclAddress();
				// TODO: maybe should transpose the matrix ?
				memcpy(&acclInst.transform, &inst.transform, sizeof(VkTransformMatrixKHR));
			}

			// TODO: what if we exceed space ?
			uint32 instSize = (uint32)geometryInstances.size() * sizeof(VkAccelerationStructureInstanceKHR);
			uint8* stageBufferData = (uint8*)stage->data + stage->offset;
			memcpy(stageBufferData, geometryInstances.data(), geometryInstances.size() * sizeof(VkAccelerationStructureInstanceKHR));
			VkBufferCopy bufferCopy{ stage->offset, 0, instSize };
			vkCmdCopyBuffer(cmdBuff, stage->stagingBuffer, tlas->GetInstanceBuffer()->GetApiObject(), 1, &bufferCopy);
			stage->offset += instSize;

			// Create VkAccelerationStructureGeometryInstancesDataKHR
			// This wraps a device pointer to the above uploaded instances.
			VkAccelerationStructureGeometryInstancesDataKHR instancesVk{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR };
			instancesVk.arrayOfPointers = VK_FALSE;
			instancesVk.data.deviceAddress = renderDevice.GetBufferAddress(tlas->GetInstanceBuffer());

			// Put the above into a VkAccelerationStructureGeometryKHR. We need to put the
			// instances struct in a union and label it as instance data.
			VkAccelerationStructureGeometryKHR topASGeometry{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
			topASGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
			topASGeometry.geometry.instances = instancesVk;

			buildInfo.geometryCount = 1;
			buildInfo.pGeometries = &topASGeometry;
			
			// Make sure the copy of the instance buffer are copied before triggering the
			// acceleration structure build
			VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
			vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
				0, 1, &barrier, 0, NULL, 0, NULL);

			// Update build information
			buildInfo.srcAccelerationStructure = update ? tlas->GetApiObject() : VK_NULL_HANDLE;
			buildInfo.dstAccelerationStructure = tlas->GetApiObject();
			buildInfo.scratchData.deviceAddress = stage->address;

			// Build Offsets info: n instances
			uint32 instanceCount = static_cast<uint32_t>(tlas->GetInfo().blasInstances.size());
			VkAccelerationStructureBuildRangeInfoKHR        buildOffsetInfo{ instanceCount, 0, 0, 0 };
			const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

			// Build the TLAS
			vkCmdBuildAccelerationStructuresKHR(cmdBuff, 1, &buildInfo, &pBuildOffsetInfo);
		}

		vkEndCommandBuffer(cmdBuff);

		CALL_VK(
			SubmitCommandBuffer(renderDevice.GetQueue(COMPUTE), &cmdBuff, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence, renderDevice.GetDevice())
		);

		stage->submitted = true;
	}

	void StagingManager::StageBlasBuilding(Blas* blas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo,
		const VkAccelerationStructureBuildRangeInfoKHR* ranges, uint32 rangesCount, uint32 flags)
	{
		RtStaging* stage = &rtStaging[currentStaging];
		
		// wait till we get everything done to re-claim resources
		this->SyncAcclBuilding();

		// Is compaction requested?
		bool doCompaction = (flags & VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR)
			== VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
		buildInfo.scratchData.deviceAddress = stage->address;
		
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildOffset(ranges, ranges + rangesCount);
		RtStaging::Batch::BatchInfo info;
		info.buildInfo = buildInfo;
		info.buildInfo.flags = flags;
		info.cmd = this->CreateCommandBuffer(stage->cmdPool);
		info.ranges = std::move(buildOffset);
		info.blasObject = blas;
		stage->batch[doCompaction].batchInfo.emplace_back(std::move(info));
	}

	void StagingManager::BuildBlasBatch(bool compact)
	{
		RtStaging* stage = &rtStaging[currentStaging];
		RtStaging::Batch& batch = stage->batch[compact];
		auto& pBuildOffset = stage->pBuildOffset;
		size_t nbBlas = batch.batchInfo.size();

		// Sanity check:
		if (batch.batchInfo.size() == 0) {
			return;
		}

		std::vector<VkCommandBuffer> commands(nbBlas);

		if (compact) {
			if (batch.queryPool)
				vkDestroyQueryPool(renderDevice.GetDevice(), batch.queryPool, NULL);

			VkQueryPoolCreateInfo qpci{ VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
			qpci.queryCount = (uint32)nbBlas;
			qpci.queryType = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
			vkCreateQueryPool(renderDevice.GetDevice(), &qpci, NULL, &batch.queryPool);
		}

		// TODO: check if we have enough memory for scratch space...

		for (size_t i = 0; i < nbBlas; i++) {
			auto& info = batch.batchInfo[i];

			if (pBuildOffset.size() <= info.ranges.size()) {
				pBuildOffset.resize(info.ranges.size() * 2);
			}

			for (size_t j = 0; j < info.ranges.size(); j++) {
				pBuildOffset[j] = &info.ranges[j];
			}

			vkCmdBuildAccelerationStructuresKHR(info.cmd, 1, &info.buildInfo, pBuildOffset.data());

			// Since the scratch buffer is reused across builds, we need a barrier to ensure one build
			// is finished before starting the next one
			VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
			barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
			barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			vkCmdPipelineBarrier(info.cmd, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
				VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, NULL, 0, NULL);

			// Write compacted size to query number idx.
			if (compact) {
				vkCmdWriteAccelerationStructuresPropertiesKHR(info.cmd, 1, &info.buildInfo.dstAccelerationStructure,
					VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR, batch.queryPool, (uint32)i);
			}

			vkEndCommandBuffer(info.cmd);
			commands[i] = info.cmd;
		}

		CALL_VK(
			SubmitCommandBuffer(renderDevice.GetQueue(COMPUTE), commands.data(), (uint32)commands.size(),
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence, renderDevice.GetDevice())
		);

		stage->submitted = true;
	}

	void StagingManager::CompressBatch(RenderBackend& backend)
	{
		RtStaging* stage = &rtStaging[currentStaging];
		RtStaging::Batch& batch = stage->batch[1]; // compact batch
		auto& compactSizes = stage->compactSizes;
		auto& cleanupAS = stage->cleanupAS;
		auto nbBlas = batch.batchInfo.size();
		bool fenceSet = stage->submitted;

		// Sanity check:
		if (batch.batchInfo.size() == 0) {
			return;
		}

		if (fenceSet) {
			printf("Waiting for fence\n");
			vkWaitForFences(renderDevice.GetDevice(), 1, &stage->fence, VK_TRUE, UINT64_MAX);
			vkResetFences(renderDevice.GetDevice(), 1, &stage->fence);
			stage->submitted = false;

			// vkResetCommandPool(renderDevice.GetDevice(), stage->cmdPool, 0);
			stage->compressionCommand = this->CreateCommandBuffer(stage->cmdPool);
		} else {
			ASSERTF(true, "Fatal Error: Fence is not set!");
			return;
		}

		if (compactSizes.size() <= nbBlas) {
			compactSizes.resize(nbBlas * 2);
		}

		if (cleanupAS.size() <= nbBlas) {
			cleanupAS.resize(nbBlas * 2);
		}

		vkGetQueryPoolResults(renderDevice.GetDevice(), batch.queryPool, 0, 
			(uint32_t)nbBlas, nbBlas * sizeof(VkDeviceSize),
			compactSizes.data(), sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT);

		for (uint32_t idx = 0; idx < nbBlas; idx++) {
			auto& info = batch.batchInfo[idx];

			// Creating a compact version of the AS
			BufferHandle buffHandle;
			VkAccelerationStructureCreateInfoKHR asCreateInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
			asCreateInfo.size = compactSizes[idx];
			asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			auto as = backend.CreateAcceleration(asCreateInfo, &buffHandle);

			// Copy the original BLAS to a compact version
			VkCopyAccelerationStructureInfoKHR copyInfo{ VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR };
			copyInfo.src = info.blasObject->GetApiObject();
			copyInfo.dst = as;
			copyInfo.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
			vkCmdCopyAccelerationStructureKHR(stage->compressionCommand, &copyInfo);

			info.blasObject->apiBlas = as;
			info.blasObject->buffer = buffHandle;

			cleanupAS[idx] = copyInfo.src;

			vkEndCommandBuffer(stage->compressionCommand);
		}

		CALL_VK(
			SubmitCommandBuffer(renderDevice.GetQueue(COMPUTE), &stage->compressionCommand, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence, renderDevice.GetDevice())
		);

		stage->submitted = true;
	}

	void StagingManager::BuildAll(RenderBackend& backend)
	{
		this->BuildBlasBatchs();
		this->CompressBatch(backend);
		this->BuildTlasBatch();
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

		vkEndCommandBuffer(cmdBuff);
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
			SubmitCommandBuffer(renderDevice.GetQueue(TRANSFER), &cmdBuff, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence, renderDevice.GetDevice());
		} else {
			SubmitCommandBuffer(renderDevice.GetQueue(TRANSFER), &cmdBuff, 1,
				0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->transferFence, renderDevice.GetDevice());
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

	VkResult StagingManager::SubmitCommandBuffer(VkQueue queue, VkCommandBuffer* cmdBuff, uint32 cmdCount, VkPipelineStageFlags waitStage,
		VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkDevice device)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.commandBufferCount = cmdCount;
		submitInfo.pCommandBuffers = cmdBuff;


		if (waitSemaphore != VK_NULL_HANDLE) {
			submitInfo.waitSemaphoreCount	= 1;
			submitInfo.pWaitSemaphores		= &waitSemaphore;
			submitInfo.pWaitDstStageMask	= &waitStage;
		}

		if (signalSemaphore != VK_NULL_HANDLE) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores	= &signalSemaphore;
		}

		return vkQueueSubmit(queue, 1, &submitInfo, fence);
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

	VkCommandBuffer StagingManager::CreateCommandBuffer(VkCommandPool pool)
	{
		// Command buffer allocation:
		VkCommandBuffer cmd;
		VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = pool;
		allocInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(renderDevice.GetDevice(), &allocInfo, &cmd);

		// Begin recording
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		beginInfo.pInheritanceInfo = NULL;
		vkBeginCommandBuffer(cmd, &beginInfo);

		return cmd;
	}
}

TRE_NS_END