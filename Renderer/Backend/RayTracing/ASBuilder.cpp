#include "ASBuilder.hpp"
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/CommandList/CommandList.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RayTracing/TLAS/TLAS.hpp>

TRE_NS_START

Renderer::AsBuilder::AsBuilder(RenderDevice& device) :
	renderDevice(device), maxScratchSize(MAX_UPLOAD_BUFFER_SIZE), currentStaging(0)
{
}

// RT functionality:
void Renderer::AsBuilder::Init()
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
        rtStaging[i].scratchBuffer = renderDevice.CreateBufferHelper(info);
        rtStaging[i].address = VK_NULL_HANDLE;
		rtStaging[i].submitted = false;
        rtStaging[i].stagingBuffer = renderDevice.CreateBufferHelper(stagingInfo);

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

void Renderer::AsBuilder::BuildBlasBatchs()
{
	// Start by building compact ones as they have dependcies later
	this->BuildBlasBatch(1);
	this->BuildBlasBatch(0);
}

void Renderer::AsBuilder::SyncAcclBuilding()
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

void Renderer::AsBuilder::StageTlasBuilding(Tlas* tlas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo,
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

void Renderer::AsBuilder::BuildTlasBatch()
{
	RtStaging* stage = &rtStaging[currentStaging];
	bool fenceSet = stage->submitted;
	size_t nbTlas = stage->tlasBuilds.size();
	auto cmdBuff = renderDevice.CreateCmdBuffer(stage->cmdPool);

	if (fenceSet) {
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

    auto queue = renderDevice.GetQueue(COMPUTE);
    CALL_VK(renderDevice.SubmitCmdBuffer(queue, &cmdBuff, 1, 0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence));

	stage->submitted = true;
}

void Renderer::AsBuilder::StageBlasBuilding(Blas* blas, VkAccelerationStructureBuildGeometryInfoKHR& buildInfo,
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
	info.cmd = renderDevice.CreateCmdBuffer(stage->cmdPool);
	info.ranges = std::move(buildOffset);
	info.blasObject = blas;
	stage->batch[doCompaction].batchInfo.emplace_back(std::move(info));
}

void Renderer::AsBuilder::BuildBlasBatch(bool compact)
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

		// vkEndCommandBuffer(info.cmd);
		commands[i] = info.cmd;
	}

    auto queue = renderDevice.GetQueue(COMPUTE);
    CALL_VK(renderDevice.SubmitCmdBuffer(queue, commands.data(), (uint32)commands.size(), 0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence));

	stage->submitted = true;
}

void Renderer::AsBuilder::CompressBatch()
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
		vkWaitForFences(renderDevice.GetDevice(), 1, &stage->fence, VK_TRUE, UINT64_MAX);
		vkResetFences(renderDevice.GetDevice(), 1, &stage->fence);
		stage->submitted = false;

		// vkResetCommandPool(renderDevice.GetDevice(), stage->cmdPool, 0);
		stage->compressionCommand = renderDevice.CreateCmdBuffer(stage->cmdPool);
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
        auto as = renderDevice.CreateAcceleration(asCreateInfo, &buffHandle);

		// Copy the original BLAS to a compact version
		VkCopyAccelerationStructureInfoKHR copyInfo{ VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR };
		copyInfo.src = info.blasObject->GetApiObject();
		copyInfo.dst = as;
		copyInfo.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
		vkCmdCopyAccelerationStructureKHR(stage->compressionCommand, &copyInfo);

		info.blasObject->apiBlas = as;
		info.blasObject->buffer = buffHandle;

		cleanupAS[idx] = copyInfo.src;

		// vkEndCommandBuffer(stage->compressionCommand);
	}


    auto queue = renderDevice.GetQueue(COMPUTE);
    CALL_VK(renderDevice.SubmitCmdBuffer(queue, &stage->compressionCommand, 1, 0, VK_NULL_HANDLE, VK_NULL_HANDLE, stage->fence));

	stage->submitted = true;
}

void Renderer::AsBuilder::BuildAll()
{
	this->BuildBlasBatchs();
    this->CompressBatch();
	this->BuildTlasBatch();
}

void Renderer::AsBuilder::Shutdown()
{
    // TODO: finish this!
}

TRE_NS_END
