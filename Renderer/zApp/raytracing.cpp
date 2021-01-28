#include "pch.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Misc/Color/Color.hpp>
#include <Renderer/Core/Alignement/Alignement.hpp>

#include "Shared.hpp"
#include "raytracing.hpp"

#define CUBE
#define STOP_POINT(str) printf(str); printf("\n"); getchar();

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
};

using namespace TRE::Renderer;

void updateCameraUBO(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::BufferHandle buffer, CameraUBO& ubo)
{
    const TRE::Renderer::Swapchain::SwapchainData& swapchainData = backend.GetRenderContext().GetSwapchain().GetSwapchainData();
    ubo.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    ubo.proj[1][1] *= -1;

    ubo.viewInverse = glm::inverse(ubo.view);
    ubo.projInverse = glm::inverse(ubo.proj);

    //ubo.viewInverse = glm::transpose(ubo.viewInverse);
    //ubo.projInverse = glm::transpose(ubo.projInverse);
    buffer->WriteToBuffer(sizeof(ubo), &ubo);
}

// Holds data for a ray tracing scratch buffer that is used as a temporary storage
struct RayTracingScratchBuffer
{
    uint64_t deviceAddress = 0;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct AccelerationStructure
{
    VkAccelerationStructureKHR handle;
    uint64_t deviceAddress = 0;
    VkDeviceMemory memory;
    VkBuffer buffer;
};

RayTracingScratchBuffer createScratchBuffer(RenderDevice& dev, VkDeviceSize size)
{
    RayTracingScratchBuffer scratchBuffer{};

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(dev.GetDevice(), &bufferCreateInfo, nullptr, &scratchBuffer.handle);

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(dev.GetDevice(), scratchBuffer.handle, &memoryRequirements);

    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = dev.FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, MemoryUsage::GPU_ONLY);
    vkAllocateMemory(dev.GetDevice(), &memoryAllocateInfo, nullptr, &scratchBuffer.memory);
    vkBindBufferMemory(dev.GetDevice(), scratchBuffer.handle, scratchBuffer.memory, 0);

    VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAddressInfo.buffer = scratchBuffer.handle;
    scratchBuffer.deviceAddress = vkGetBufferDeviceAddressKHR(dev.GetDevice(), &bufferDeviceAddressInfo);

    return scratchBuffer;
}

void createAccelerationStructureBuffer(RenderDevice& dev, AccelerationStructure& accelerationStructure, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(dev.GetDevice(), &bufferCreateInfo, nullptr, &accelerationStructure.buffer);
    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(dev.GetDevice(), accelerationStructure.buffer, &memoryRequirements);
    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = dev.FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, MemoryUsage::GPU_ONLY);
    vkAllocateMemory(dev.GetDevice(), &memoryAllocateInfo, nullptr, &accelerationStructure.memory);
    vkBindBufferMemory(dev.GetDevice(), accelerationStructure.buffer, accelerationStructure.memory, 0);
}

VkPipelineShaderStageCreateInfo loadShader(VkDevice dev, std::string fileName, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;
    auto shaderCode = TRE::Renderer::ReadShaderFile(fileName);
    shaderStage.module = TRE::Renderer::ShaderProgram::CreateShaderModule(dev, shaderCode);
    shaderStage.pName = "main";
    assert(shaderStage.module != VK_NULL_HANDLE);
    // shaderModules.push_back(shaderStage.module);
    return shaderStage;
}

/*
        Create our ray tracing pipeline
    */
void createRayTracingPipeline(RenderBackend& backend, VkPipeline& pipeline, SBT& sbt)
{
    VkPipelineLayout layout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;

    auto device = backend.GetRenderDevice().GetDevice();
    auto renderDevice = backend.GetRenderDevice();
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};

    VkDescriptorSetLayoutBinding accelerationStructureLayoutBinding{};
    accelerationStructureLayoutBinding.binding = 0;
    accelerationStructureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    accelerationStructureLayoutBinding.descriptorCount = 1;
    accelerationStructureLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    VkDescriptorSetLayoutBinding resultImageLayoutBinding{};
    resultImageLayoutBinding.binding = 1;
    resultImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    resultImageLayoutBinding.descriptorCount = 1;
    resultImageLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    VkDescriptorSetLayoutBinding uniformBufferBinding{};
    uniformBufferBinding.binding = 2;
    uniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferBinding.descriptorCount = 1;
    uniformBufferBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    std::vector<VkDescriptorSetLayoutBinding> bindings({
        accelerationStructureLayoutBinding,
        resultImageLayoutBinding,
        uniformBufferBinding
        });

    VkDescriptorSetLayoutCreateInfo descriptorSetlayoutCI{};
    descriptorSetlayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetlayoutCI.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetlayoutCI.pBindings = bindings.data();
    vkCreateDescriptorSetLayout(device, &descriptorSetlayoutCI, nullptr, &descriptorSetLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &descriptorSetLayout;
    vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout);

    /*
        Setup ray tracing shader groups
    */
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    // Ray generation group
    {
        shaderStages.push_back(loadShader(device, "shaders/RT/rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
        VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
        shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
        shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
        shaderGroups.push_back(shaderGroup);
    }

    // Miss group
    {
        shaderStages.push_back(loadShader(device, "shaders/RT/rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
        VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
        shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
        shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
        shaderGroups.push_back(shaderGroup);
    }

    // Closest hit group
    {
        shaderStages.push_back(loadShader(device, "shaders/RT/rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
        VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
        shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
        shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
        shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
        shaderGroups.push_back(shaderGroup);
    }

    /*
        Create the ray tracing pipeline
    */
    VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCI{};
    rayTracingPipelineCI.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    rayTracingPipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
    rayTracingPipelineCI.pStages = shaderStages.data();
    rayTracingPipelineCI.groupCount = static_cast<uint32_t>(shaderGroups.size());
    rayTracingPipelineCI.pGroups = shaderGroups.data();
    rayTracingPipelineCI.maxPipelineRayRecursionDepth = 1;
    rayTracingPipelineCI.layout = pipelineLayout;
    vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &rayTracingPipelineCI, nullptr, &pipeline);

    // SBT:
    auto groupCount = 3;

    uint32_t groupHandleSize = renderDevice.GetRtProperties().shaderGroupHandleSize;  // Size of a program identifier
    uint32_t baseAlignment = renderDevice.GetRtProperties().shaderGroupBaseAlignment;  // Size of shader alignment
    // Compute the actual size needed per SBT entry (round-up to alignment needed).
    uint32_t groupSizeAligned = TRE::Renderer::Utils::AlignUp(groupHandleSize, baseAlignment);

    // Fetch all the shader handles used in the pipeline, so that they can be written in the SBT
    uint32_t sbtSize = groupCount * groupSizeAligned;

    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());

    BufferInfo sbtBufferInfo;
    sbtBufferInfo.domain = MemoryUsage::CPU_COHERENT;
    sbtBufferInfo.usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    sbtBufferInfo.size = groupSizeAligned;

    for (uint32 i = 0; i < 3; i++) {
        sbt.sbtBuffer[i] = backend.CreateBuffer(sbtBufferInfo);
        sbt.sbtBuffer[i]->WriteToBuffer(groupHandleSize, shaderHandleStorage.data() + groupSizeAligned * i);
        sbt.address[i] = renderDevice.GetBufferAddress(sbt.sbtBuffer[i]);
        // deviceAddress;stride;size;
        sbt.sbtEntries[i] = { sbt.address[i], groupSizeAligned, groupSizeAligned };
    }
    sbt.sbtEntries[3] = { 0,0,0 };

    printf("Pipeline created!\n");
}

/*
    Create the bottom level acceleration structure contains the scene's actual geometry (vertices, triangles)
*/
void createAccelerationStructure(RenderBackend& backend, AccelerationStructure& blas, AccelerationStructure& tlas)
{
    auto device = backend.GetRenderDevice().GetDevice();

    // Setup vertices for a single triangle
    struct Vertex
    {
        float pos[3];
    };

    std::vector<Vertex> vertices = {
        { {  1.0f,  1.0f, 0.0f } },
        { { -1.0f,  1.0f, 0.0f } },
        { {  0.0f, -1.0f, 0.0f } }
    };

    // Setup indices
    std::vector<uint32_t> indices = { 0, 1, 2 };
    uint32 indexCount = static_cast<uint32_t>(indices.size());

    // Setup identity transform matrix
    VkTransformMatrixKHR transformMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };

    // Create buffers
    // For the sake of simplicity we won't stage the vertex data to the GPU memory
    // Vertex buffer
    BufferInfo bufferInfo;
    bufferInfo.domain = MemoryUsage::CPU_COHERENT;
    bufferInfo.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
    bufferInfo.size = vertices.size() * sizeof(Vertex);
    BufferHandle vertexBuffer = backend.CreateBuffer(bufferInfo, vertices.data());

    // Index buffer
    bufferInfo.size = indices.size() * sizeof(uint32_t);
    BufferHandle indexBuffer = backend.CreateBuffer(bufferInfo, indices.data());

    // Transform buffer
    bufferInfo.size = sizeof(VkTransformMatrixKHR);
    BufferHandle transformMatrixBuffer = backend.CreateBuffer(bufferInfo, &transformMatrix);

    VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress{};

    vertexBufferDeviceAddress.deviceAddress = backend.GetRenderDevice().GetBufferAddress(vertexBuffer);
    indexBufferDeviceAddress.deviceAddress = backend.GetRenderDevice().GetBufferAddress(indexBuffer);
    transformBufferDeviceAddress.deviceAddress = backend.GetRenderDevice().GetBufferAddress(transformMatrixBuffer);

    // Build
    VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
    accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    accelerationStructureGeometry.geometry.triangles.vertexData = vertexBufferDeviceAddress;
    accelerationStructureGeometry.geometry.triangles.maxVertex = 3;
    accelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
    accelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
    accelerationStructureGeometry.geometry.triangles.indexData = indexBufferDeviceAddress;
    accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
    accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;
    accelerationStructureGeometry.geometry.triangles.transformData = transformBufferDeviceAddress;

    // Get size info
    VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
    accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    accelerationStructureBuildGeometryInfo.geometryCount = 1;
    accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

    const uint32_t numTriangles = 1;
    VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
    accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        backend.GetRenderDevice().GetDevice(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &accelerationStructureBuildGeometryInfo,
        &numTriangles,
        &accelerationStructureBuildSizesInfo);

    createAccelerationStructureBuffer(backend.GetRenderDevice(), blas, accelerationStructureBuildSizesInfo);

    VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
    accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    accelerationStructureCreateInfo.buffer = blas.buffer;
    accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
    accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    vkCreateAccelerationStructureKHR(backend.GetRenderDevice().GetDevice(), &accelerationStructureCreateInfo, nullptr, &blas.handle);

    // Create a small scratch buffer used during build of the bottom level acceleration structure
    RayTracingScratchBuffer scratchBuffer = createScratchBuffer(backend.GetRenderDevice(), accelerationStructureBuildSizesInfo.buildScratchSize);

    VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
    accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    accelerationBuildGeometryInfo.dstAccelerationStructure = blas.handle;
    accelerationBuildGeometryInfo.geometryCount = 1;
    accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
    accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;

    VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
    accelerationStructureBuildRangeInfo.primitiveCount = numTriangles;
    accelerationStructureBuildRangeInfo.primitiveOffset = 0;
    accelerationStructureBuildRangeInfo.firstVertex = 0;
    accelerationStructureBuildRangeInfo.transformOffset = 0;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

    if (backend.GetRenderDevice().GetAcclFeatures().accelerationStructureHostCommands) {
        // Implementation supports building acceleration structure building on host
        vkBuildAccelerationStructuresKHR(
            backend.GetRenderDevice().GetDevice(),
            VK_NULL_HANDLE,
            1,
            &accelerationBuildGeometryInfo,
            accelerationBuildStructureRangeInfos.data());
    } else {
        // Acceleration structure needs to be build on the device
        auto commandBuffer = backend.RequestCommandBuffer(CommandBuffer::Type::ASYNC_COMPUTE);

        vkCmdBuildAccelerationStructuresKHR(
            commandBuffer->GetApiObject(),
            1,
            &accelerationBuildGeometryInfo,
            accelerationBuildStructureRangeInfos.data());

        FenceHandle fence;
        backend.Submit(commandBuffer, &fence);
        fence->Wait();
    }

    VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
    accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    accelerationDeviceAddressInfo.accelerationStructure = blas.handle;
    blas.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(backend.GetRenderDevice().GetDevice(), &accelerationDeviceAddressInfo);

    // deleteScratchBuffer(scratchBuffer);

    // TLAS:
    {
        /*VkTransformMatrixKHR transformMatrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f };*/

        VkAccelerationStructureInstanceKHR instance{};
        instance.transform = transformMatrix;
        instance.instanceCustomIndex = 0;
        instance.mask = 0xFF;
        instance.instanceShaderBindingTableRecordOffset = 0;
        instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        instance.accelerationStructureReference = blas.deviceAddress;

        // Buffer for instance data
        BufferInfo bufferInfo2;
        bufferInfo2.domain = MemoryUsage::CPU_COHERENT;
        bufferInfo2.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        bufferInfo2.size = sizeof(VkAccelerationStructureInstanceKHR);
        BufferHandle instancesBuffer = backend.CreateBuffer(bufferInfo2, &instance);

        VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
        instanceDataDeviceAddress.deviceAddress = backend.GetRenderDevice().GetBufferAddress(instancesBuffer);

        VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
        accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        accelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        accelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
        accelerationStructureGeometry.geometry.instances.data = instanceDataDeviceAddress;

        // Get size info
        /*
        The pSrcAccelerationStructure, dstAccelerationStructure, and mode members of pBuildInfo are ignored. Any VkDeviceOrHostAddressKHR members of pBuildInfo are ignored by this command, except that the hostAddress member of VkAccelerationStructureGeometryTrianglesDataKHR::transformData will be examined to check if it is NULL.*
        */
        VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
        accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        accelerationStructureBuildGeometryInfo.geometryCount = 1;
        accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

        uint32_t primitive_count = 1;

        VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
        accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vkGetAccelerationStructureBuildSizesKHR(
            device,
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &accelerationStructureBuildGeometryInfo,
            &primitive_count,
            &accelerationStructureBuildSizesInfo);

        createAccelerationStructureBuffer(backend.GetRenderDevice(), tlas, accelerationStructureBuildSizesInfo);

        VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
        accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        accelerationStructureCreateInfo.buffer = tlas.buffer;
        accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
        accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        vkCreateAccelerationStructureKHR(device, &accelerationStructureCreateInfo, nullptr, &tlas.handle);

        // Create a small scratch buffer used during build of the top level acceleration structure
        RayTracingScratchBuffer scratchBuffer = createScratchBuffer(backend.GetRenderDevice(), accelerationStructureBuildSizesInfo.buildScratchSize);

        VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
        accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        accelerationBuildGeometryInfo.dstAccelerationStructure = tlas.handle;
        accelerationBuildGeometryInfo.geometryCount = 1;
        accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
        accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;

        VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
        accelerationStructureBuildRangeInfo.primitiveCount = 1;
        accelerationStructureBuildRangeInfo.primitiveOffset = 0;
        accelerationStructureBuildRangeInfo.firstVertex = 0;
        accelerationStructureBuildRangeInfo.transformOffset = 0;
        std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

        if (backend.GetRenderDevice().GetAcclFeatures().accelerationStructureHostCommands) {
            // Implementation supports building acceleration structure building on host
            vkBuildAccelerationStructuresKHR(
                device,
                VK_NULL_HANDLE,
                1,
                &accelerationBuildGeometryInfo,
                accelerationBuildStructureRangeInfos.data());
        } else {
            // Acceleration structure needs to be build on the device
            auto commandBuffer = backend.RequestCommandBuffer(CommandBuffer::Type::ASYNC_COMPUTE);

            vkCmdBuildAccelerationStructuresKHR(
                commandBuffer->GetApiObject(),
                1,
                &accelerationBuildGeometryInfo,
                accelerationBuildStructureRangeInfos.data());

            FenceHandle fence;
            backend.Submit(commandBuffer, &fence);
            fence->Wait();
        }

        VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
        accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        accelerationDeviceAddressInfo.accelerationStructure = tlas.handle;
        tlas.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &accelerationDeviceAddressInfo);
    }
}

int rt()
{
    const unsigned int SCR_WIDTH = 640; //1920 / 2;
    const unsigned int SCR_HEIGHT = 480; //1080 / 2;

    using namespace TRE::Renderer;
    using namespace TRE;

    Event ev;
    Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderBackend backend{ &window };
    backend.InitInstance(RenderBackend::RAY_TRACING);
    // backend.SetSamplerCount(2);

    if (backend.GetMSAASamplerCount() == 1) {
        TRE_LOGI("Anti-Aliasing: Disabled");
    } else {
        TRE_LOGI("Anti-Aliasing: MSAA x%d", backend.GetMSAASamplerCount());
    }

    TRE_LOGI("Engine is up and running ...");
    //STOP_POINT("ENGINE INIT");
    

#if !defined(CUBE)
    size_t vertexSize = sizeof(vertices[0]) * vertices.size();
    size_t indexSize = sizeof(indices[0]) * indices.size();
    char* data = new char[vertexSize + indexSize];
    memcpy(data, vertices.data(), vertexSize);
    memcpy(data + vertexSize, indices.data(), indexSize);

    BufferHandle vertexIndexBuffer = backend.CreateBuffer(
        { vertexSize + indexSize, BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER | BufferUsage::INDEX_BUFFER },
        data);
#else
    Vertex vertecies[12 * 3];;

    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = TRE::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = TRE::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = TRE::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    std::vector<uint32> indicies(12 * 3);
    for (uint32 i = 0; i < 12 * 3; i++)
        indicies[i] = i;
#endif
    GraphicsState state;

    BufferHandle ubo = backend.CreateBuffer(BufferInfo::UniformBuffer(sizeof(CameraUBO)));
    CameraUBO uboData{};
    updateCameraUBO(backend, ubo, uboData);
    //ubo = backend.CreateBuffer({ sizeof(CameraUBO), BufferUsage::UNIFORM_BUFFER, MemoryUsage::GPU_ONLY }, &uboData);

    ImageHandle rtImage[3];
    ImageViewHandle rtView[3];

    for (uint32 i = 0; i < 3; i++) {
        rtImage[i] = backend.CreateImage(ImageCreateInfo::RtRenderTarget(SCR_WIDTH, SCR_HEIGHT));
        rtView[i] = backend.CreateImageView(ImageViewCreateInfo::ImageView(rtImage[i]));
    }
    
    // Out texture
    SamplerHandle outTexture = backend.CreateSampler(SamplerInfo::Sampler2DV2(rtImage[0]));

    BufferHandle acclBuffer = backend.CreateBuffer(
        { sizeof(vertecies),
        BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER
        | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    BufferHandle acclIndexBuffer = backend.CreateBuffer(
        { indicies.size() * sizeof(uint32),
        BufferUsage::INDEX_BUFFER | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    backend.GetStagingManager().Stage(acclBuffer->GetApiObject(), &vertecies, sizeof(vertecies));
    backend.GetStagingManager().Stage(acclIndexBuffer->GetApiObject(), indicies.data(), indicies.size() * sizeof(uint32));
    backend.GetStagingManager().Flush();
    backend.GetStagingManager().WaitCurrent();

    //STOP_POINT("Staging Manager Flushing Buffer");

    /*VkDeviceAddress vertexData,
			VkDeviceSize vertexStride, uint32 vertexCount,
			VkDeviceAddress transformData = VK_NULL_HANDLE,
			VkDeviceAddress indexData = VK_NULL_HANDLE,
			const AsOffset& offset = {0, 0, 0, 0},
			uint32 flags = VK_GEOMETRY_OPAQUE_BIT_KHR,
			VkFormat vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
			VkIndexType indexType = VK_INDEX_TYPE_UINT32*/

    /*BlasCreateInfo info;
    info.AddGeometry(
        backend.GetRenderDevice().GetBufferAddress(acclBuffer),
        sizeof(Vertex), 12*3, NULL, 
        backend.GetRenderDevice().GetBufferAddress(acclIndexBuffer), 
        { 12, 0, 0, 0 }
    );

    BlasHandle blas = backend.CreateBlas(info, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
    //VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
    backend.RtBuildBlasBatchs();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtCompressBatch();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtSyncAcclBuilding();

    //STOP_POINT("Building Accl Structure");

    BlasInstance instance;
    instance.blas = blas;
    auto t = glm::mat4(1.f);
    t = glm::transpose(t);
    memcpy(instance.transform, &t, sizeof(glm::mat4));
    TlasCreateInfo tlasInfo;
    tlasInfo.blasInstances.emplace_back(instance);
    TlasHandle tlas = backend.CreateTlas(tlasInfo);
    backend.BuildAS();
    backend.RtSyncAcclBuilding();*/

    ShaderProgram rtProgram(backend,
        {
            {"shaders/RT/rgen.spv", ShaderProgram::RGEN},
            {"shaders/RT/rmiss.spv", ShaderProgram::RMISS},
            {"shaders/RT/rchit.spv", ShaderProgram::RCHIT},
        });
    rtProgram.Compile();
    Pipeline rtPipeline(PipelineType::RAY_TRACE, &rtProgram);
    rtPipeline.Create(backend, 2);

    /*ShaderProgram postProgram(backend,
        {
            {"shaders/Post/post.vert.spv", ShaderProgram::VERTEX},
            {"shaders/Post/post.frag.spv", ShaderProgram::FRAGMENT},
        });
    postProgram.Compile();*/

    // STOP_POINT("Building Accl Structure (TLAS)");
    AccelerationStructure blas; AccelerationStructure tlas;
    createAccelerationStructure(backend, blas, tlas);

    VkPipeline pipeline;
    SBT sbt;
    createRayTracingPipeline(backend, pipeline, sbt);
    INIT_BENCHMARK;

    time_t lasttime = time(NULL);
    // TODO: shader specilization constants 

    // D:/EngineDev/TrikytaEngine3D/Build/Renderer/x64/Debug/Renderer.exe
    while (window.isOpen()) {
        window.getEvent(ev);

        if (ev.Type == TRE::Event::TE_RESIZE) {
            backend.GetRenderContext().GetSwapchain().UpdateSwapchain();
            continue;
        } else if (ev.Type == TRE::Event::TE_KEY_UP) {
            if (ev.Key.Code == TRE::Key::L) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_LINE;
                state.SaveChanges();
            } else if (ev.Key.Code == TRE::Key::F) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_FILL;
                state.SaveChanges();
            }
        }
        
        backend.BeginFrame();
        //STOP_POINT("Begin Frame");
        uint32 frame = backend.GetRenderContext().GetCurrentImageIndex();
        RenderContext& ctx = backend.GetRenderContext();

        auto cmd = backend.RequestCommandBuffer(CommandBuffer::Type::RAY_TRACING);
        /*RenderPassInfo::Subpass subpass;
        cmd->BeginRenderPass(GetRenderPass(backend, subpass));
        cmd->EndRenderPass();*/
        /*RenderPassInfo::Subpass subpass;
        cmd->BeginRenderPass(GetRenderPass(backend, subpass));
        cmd->EndRenderPass();*/

        cmd->BindShaderProgram(rtProgram);
        cmd->BindPipeline(rtPipeline);
        cmd->SetAccelerationStrucure(0, 0, &tlas.handle);
        cmd->SetTexture(0, 1, *rtView[frame]);
        cmd->SetUniformBuffer(0, 2, *ubo);
        // cmd->PushConstants(VK_SHADER_STAGE_RAYGEN_BIT_KHR, &uboData, sizeof(uboData));
        cmd->TraceRays(pipeline, sbt, SCR_WIDTH, SCR_HEIGHT, 1);
        //SemaphoreHandle semaphore;
        //backend.Submit(cmd, NULL, 1, &semaphore);
        //backend.AddWaitSemapore(CommandBuffer::Type::GENERIC, semaphore, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        cmd->ChangeImageLayout(*ctx.GetCurrentSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
        cmd->ChangeImageLayout(*rtImage[frame], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange);

        cmd->CopyImage(*rtImage[frame], *ctx.GetCurrentSwapchainImage(), { SCR_WIDTH, SCR_HEIGHT, 1 } );

        cmd->ChangeImageLayout(*ctx.GetCurrentSwapchainImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, subresourceRange);
        cmd->ChangeImageLayout(*rtImage[frame], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, subresourceRange);

        //STOP_POINT("Submitting RT CMD");

        /*auto cmd1 = backend.RequestCommandBuffer(CommandBuffer::Type::GENERIC);
        cmd1->BindShaderProgram(postProgram);
        //state.GetInputAssemblyState().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        //state.SaveChanges();
        cmd1->SetGraphicsState(state);
        cmd1->SetViewport({ 0, 0, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0, 1 });
        cmd1->SetScissor({ {0, 0}, {SCR_WIDTH, SCR_HEIGHT} });
        RenderPassInfo::Subpass subpass;
        cmd1->BeginRenderPass(GetRenderPass(backend, subpass));
        cmd1->SetTexture(0, 0, *rtView[frame], *outTexture);
        //cmd1->SetTexture(0, 0, *textureView, *sampler);
        cmd1->Draw(3);
        cmd1->EndRenderPass();
        backend.Submit(cmd1);*/

        //STOP_POINT("Submitting Post render CMD");
        //backend.Submit(cmd, NULL, 1, &backend.GetDrawCompletedSemaphore());
        backend.Submit(cmd);
        backend.EndFrame();
        printFPS();
    }

#if !defined(CUBE)
    delete[] data;
#endif
    getchar();
    return 0;
}