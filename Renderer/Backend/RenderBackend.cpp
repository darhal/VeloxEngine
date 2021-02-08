#include "RenderBackend.hpp"
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::RenderBackend::RenderBackend(TRE::Window* wnd) :
    stagingManager{ renderDevice },
    window(wnd),
    renderContext(*this),
    framebufferAllocator(&renderDevice),
    transientAttachmentAllocator(*this, true),
    pipelineAllocator(this),
    msaaSamplerCount(1),
    acclBuilder(renderDevice)
{
    renderDevice.internal.renderContext = &renderContext.internal;
    renderContext.internal.renderDevice = &renderDevice.internal;
}

void Renderer::RenderBackend::InitInstance(uint32 usage)
{
    enabledFeatures = usage;
    StaticVector<const char*> deviceExt;

    if (usage & RAY_TRACING) {
        for (auto ext : DEV_EXTENSIONS[GetSetBit(RAY_TRACING)]) {
            deviceExt.PushBack(ext);
        }
    }

    renderInstance.CreateRenderInstance();

    renderContext.CreateRenderContext(window, renderInstance.internal);
    renderDevice.CreateRenderDevice(renderInstance.internal, renderContext.internal, deviceExt.begin(), deviceExt.Size());
    renderContext.InitRenderContext(renderInstance.internal, renderDevice.internal);

    stagingManager.Init();
    gpuMemoryAllocator.Init(renderDevice.internal);
    fenceManager.Init(&renderDevice);
    semaphoreManager.Init(&renderDevice);
    eventManager.Init(&renderDevice);

    // RT: 
    if (usage & RAY_TRACING) 
        acclBuilder.Init();

    this->Init();

    const auto vendor = [](uint32 id) -> std::string
    {
        switch (id) {
        case 0x1002:
            return "AMD";
            break;
        case 0x10DE:
            return "NIVIDIA";
            break;
        case 0x8086:
            return "Intel";
            break;
        case 0x13B5:
            return "ARM";
            break;
        case 0x5143:
            return "Qualcomm";
            break;
        case 0x1010:
            return "Imagination Technology";
            break;
        default:
            return "Unknown";
        }
    };

    TRE_LOGI("GPU............: %s", renderDevice.internal.gpuProperties.deviceName);
    TRE_LOGI("Vendor.........: %s", vendor(renderDevice.internal.gpuProperties.vendorID).c_str());
    TRE_LOGI("Driver.........: %d.%d", VK_VERSION_MAJOR(renderDevice.internal.gpuProperties.driverVersion), VK_VERSION_MINOR(renderDevice.internal.gpuProperties.driverVersion));
    TRE_LOGI("Device ID......: 0x%x", renderDevice.internal.gpuProperties.deviceID);
}

void Renderer::RenderBackend::Init()
{
    const Internal::QueueFamilyIndices& queueFamilyIndices = renderDevice.GetQueueFamilyIndices();

    for (uint32 f = 0; f < renderContext.GetNumFrames(); f++) {
        for (uint32 t = 0; t < MAX_THREADS; t++) {
            for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
                if (queueFamilyIndices.queueFamilies[i] == UINT32_MAX) {
                    TRE_LOGW("Skipping queue familly index %d", i);
                    continue;
                }

                PerFrame& frame = perFrame[f];
                frame.commandPools[t][i] = std::move(CommandPool(&renderDevice, queueFamilyIndices.queueFamilies[i]));
            }
        }
    }
}

Renderer::RenderBackend::~RenderBackend()
{
    vkDeviceWaitIdle(renderDevice.internal.device);

    renderContext.DestroyRenderContext(renderInstance.internal, renderDevice.internal, renderContext.internal);
    this->Shutdown();
    renderDevice.DestroryRenderDevice();
    renderInstance.DestroyRenderInstance();
}

void Renderer::RenderBackend::Shutdown()
{
    VkDevice device =  renderDevice.GetDevice();
    for (const auto& rp : renderPasses) {
        vkDestroyRenderPass(device, rp.second.GetApiObject(), NULL);
    }

    for (auto& descSetAlloc : descriptorSetAllocators) {
        descSetAlloc.second.Destroy();
    }

    framebufferAllocator.Destroy();
    transientAttachmentAllocator.Destroy();
    pipelineAllocator.Destroy();
    fenceManager.Destroy();
    semaphoreManager.Destroy();
    eventManager.Destroy();
    gpuMemoryAllocator.Destroy();

    stagingManager.Shutdown();

    if (enabledFeatures & RAY_TRACING) {
        acclBuilder.Shutdown();
    }

    this->DestroyAllFrames();
}

Renderer::CommandBuffer::Type Renderer::RenderBackend::GetPhysicalQueueType(CommandBuffer::Type type)
{
    return type;
}

Renderer::RenderBackend::PerFrame::QueueData& Renderer::RenderBackend::GetQueueData(CommandBuffer::Type type)
{
    return Frame().queueData[(uint32)type];
}

Renderer::StaticVector<Renderer::CommandBufferHandle>& Renderer::RenderBackend::GetQueueSubmissions(CommandBuffer::Type type)
{
    return Frame().submissions[(uint32)type];
}

VkQueue Renderer::RenderBackend::GetQueue(CommandBuffer::Type type)
{
    uint32 typeIndex = (uint32)(type == CommandBuffer::Type::RAY_TRACING ? CommandBuffer::Type::GENERIC : type);
    return renderDevice.GetQueue(typeIndex);
}

void Renderer::RenderBackend::Submit(CommandBufferHandle cmd, FenceHandle* fence, uint32 semaphoreCount, 
    SemaphoreHandle* semaphores, bool swapchainSemaphore )
{
    cmd->End();

    PerFrame& frame = Frame();
    uint32 type = (uint32)cmd->GetType();
    CommandBufferHandle& handle = frame.submissions[type].EmplaceBack(std::move(cmd));

    if (fence || semaphoreCount || swapchainSemaphore) {
        this->SubmitQueue((CommandBuffer::Type)type, fence, semaphoreCount, semaphores, swapchainSemaphore);
    }
}

void Renderer::RenderBackend::SubmitQueue(CommandBuffer::Type type, FenceHandle* fence, uint32 semaphoreCount, 
    SemaphoreHandle* semaphores, bool lastSubmit)
{
    if (type != CommandBuffer::Type::ASYNC_TRANSFER)
        this->FlushQueue(CommandBuffer::Type::ASYNC_TRANSFER);

    auto& data = this->GetQueueData(type);
    auto& submissions = this->GetQueueSubmissions(type);

    if (submissions.Size() == 0) {
        if (fence || semaphoreCount) {
            // this->SubmitEmpty(type, fence, semaphoreCount, semaphores);
        }

        return;
    }

    CommandBufferHandle swapchainCommandBuffer;
    StaticVector<VkSubmitInfo> submits;
    StaticVector<VkCommandBuffer> cmds;
    StaticVector<VkSemaphore> waits;
    StaticVector<VkSemaphore> signals;
    VkFence vkFence = VK_NULL_HANDLE;
    bool swapchainResize = renderContext.GetSwapchain().ResizeRequested();

    for (auto& sem : data.waitSemaphores) {
        waits.PushBack(sem->GetApiObject());
    }

    for (auto& sub : submissions) {
        if (sub->UsesSwapchain()) {
            swapchainCommandBuffer = sub;
        }

        cmds.PushBack(sub->GetApiObject());
    }

    for (uint32 i = 0; i < semaphoreCount; i++) {
        VkSemaphore sem = semaphoreManager.RequestSemaphore();
        semaphores[i] = SemaphoreHandle(objectsPool.semaphores.Allocate(*this, sem));
        signals.EmplaceBack(sem);
    }

    if ((swapchainCommandBuffer || lastSubmit) && !swapchainResize) {
        VkPipelineStageFlagBits stage = swapchainCommandBuffer ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
            : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        data.waitStages.PushBack(stage);
        waits.EmplaceBack(renderContext.GetImageAcquiredSemaphore());
        signals.EmplaceBack(renderContext.GetDrawCompletedSemaphore());
    }

    auto& submit = submits.EmplaceBack();
    submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit.waitSemaphoreCount = waits.Size();
    submit.pWaitSemaphores = waits.Data();
    submit.pWaitDstStageMask = data.waitStages.Data();
    submit.commandBufferCount = cmds.Size();
    submit.pCommandBuffers = cmds.Data();
    submit.signalSemaphoreCount = signals.Size();
    submit.pSignalSemaphores = signals.Data();

    ASSERTF(fence && swapchainCommandBuffer, "Can't submit command buffers that draw to swapchain with a fance");

    if (fence) {
        *fence = this->RequestFence();
        vkFence = (*fence)->GetApiObject();
    }

    if (swapchainCommandBuffer || lastSubmit) {
        vkFence = renderContext.GetFrameFence();
    }

    vkQueueSubmit(this->GetQueue(type), submits.Size(), submits.Data(), vkFence);

    submissions.Clear();
    data.waitSemaphores.Clear();
    data.waitStages.Clear();
}

void Renderer::RenderBackend::AddWaitSemapore(CommandBuffer::Type type, SemaphoreHandle semaphore, VkPipelineStageFlags stages, bool flush)
{
    ASSERT(stages == 0);

    if (flush) {
        this->FlushQueue(type);
    }

    auto& data = GetQueueData(type);
    data.waitSemaphores.EmplaceBack(semaphore);
    data.waitStages.PushBack(stages);
}

void Renderer::RenderBackend::FlushQueue(CommandBuffer::Type type)
{
    this->SubmitQueue(type);
}

void Renderer::RenderBackend::FlushFrame()
{
    this->FlushQueue(CommandBuffer::Type::ASYNC_TRANSFER);
    this->FlushQueue(CommandBuffer::Type::ASYNC_COMPUTE);
    this->FlushQueue(CommandBuffer::Type::GENERIC);
}

void Renderer::RenderBackend::ClearFrame()
{
    PerFrame& frame = Frame();

    for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
        frame.commandPools[0][i].Reset();
        frame.submissions[i].Clear();
    }

    // objectsPool.commandBuffers.Clear();
    this->DestroyPendingObjects(frame);
}

void Renderer::RenderBackend::BeginFrame()
{
    PerFrame& frame = Frame();

    if (!frame.waitFences.Empty()) {
        vkWaitForFences(renderDevice.GetDevice(), frame.waitFences.Size(), frame.waitFences.Data(), VK_TRUE, UINT64_MAX);
        vkResetFences(renderDevice.GetDevice(), frame.waitFences.Size(), frame.waitFences.Data());
        frame.waitFences.Clear();
    }

    renderContext.BeginFrame(renderDevice, stagingManager);

    framebufferAllocator.BeginFrame();
    transientAttachmentAllocator.BeginFrame();

    for (auto& allocator : descriptorSetAllocators)
        allocator.second.BeginFrame();

    this->ClearFrame();
}

void Renderer::RenderBackend::EndFrame()
{
    CONSTEXPR uint32 subOrder[] = {
        (uint32)CommandBuffer::Type::ASYNC_TRANSFER,
        (uint32)CommandBuffer::Type::ASYNC_COMPUTE
    };

    PerFrame& frame = this->Frame();
    FenceHandle fence;

    for (uint32 type : subOrder) {
        if (frame.submissions[type].Size()) {
            CommandBuffer::Type qType = (CommandBuffer::Type)type;
            this->SubmitQueue(qType);
            frame.waitFences.EmplaceBack(fence->GetApiObject());
            frame.recycleFences.EmplaceBack(fence->GetApiObject());
        }
    }

    auto& submissions = this->GetQueueSubmissions(CommandBuffer::Type::GENERIC);
    if (submissions.Size() != 0) {
        this->SubmitQueue(CommandBuffer::Type::GENERIC, NULL, 0, NULL, true);
    }

    renderContext.EndFrame(renderDevice);
}

Renderer::BlasHandle Renderer::RenderBackend::CreateBlas(const BlasCreateInfo& blasInfo, VkBuildAccelerationStructureFlagsKHR flags)
{
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo;
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.pNext = NULL;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildInfo.flags = flags;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.dstAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.geometryCount  = blasInfo.acclGeo.Size();
    buildInfo.pGeometries = blasInfo.acclGeo.begin();
    buildInfo.ppGeometries = NULL;
    
    StaticVector<uint32, 256> maxPrimCount;
    maxPrimCount.Resize(blasInfo.accOffset.Size());

    for (uint32 tt = 0; tt < blasInfo.accOffset.Size(); tt++)
        maxPrimCount[tt] = blasInfo.accOffset[tt].primitiveCount;  // Number of primitives/triangles

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
    vkGetAccelerationStructureBuildSizesKHR(renderDevice.GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo, maxPrimCount.begin(), &sizeInfo);

    // Create acceleration structure object. Not yet bound to memory.
    VkAccelerationStructureCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    createInfo.size = sizeInfo.accelerationStructureSize; // Will be used to allocate memory.

    // Actual allocation of buffer and acceleration structure. Note: This relies on createInfo.offset == 0
    // and fills in createInfo.buffer with the buffer allocated to store the BLAS. The underlying
    // vkCreateAccelerationStructureKHR call then consumes the buffer value.
    BufferHandle buffer;
    VkAccelerationStructureKHR blas = this->CreateAcceleration(createInfo, &buffer);
    buildInfo.dstAccelerationStructure = blas;  // Setting the where the build lands
    BlasHandle ret(objectsPool.blases.Allocate(*this, blasInfo, blas, buffer));
    acclBuilder.StageBlasBuilding(ret, buildInfo, blasInfo.accOffset.begin(), blasInfo.accOffset.Size(), flags);
    return ret;
}

Renderer::TlasHandle Renderer::RenderBackend::CreateTlas(const TlasCreateInfo& createInfo, VkBuildAccelerationStructureFlagsKHR flags)
{
    BufferInfo bufferInfo;
    bufferInfo.size = createInfo.blasInstances.size() * sizeof(VkAccelerationStructureInstanceKHR);
    bufferInfo.usage = BufferUsage::SHADER_DEVICE_ADDRESS;
    bufferInfo.domain = MemoryUsage::GPU_ONLY;
    BufferHandle instanceBuffer = this->CreateBuffer(bufferInfo);
    VkDeviceAddress instanceAddress = renderDevice.GetBufferAddress(instanceBuffer);

    // Create VkAccelerationStructureGeometryInstancesDataKHR
    // This wraps a device pointer to the above uploaded instances.
    VkAccelerationStructureGeometryInstancesDataKHR instancesVk{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR };
    instancesVk.arrayOfPointers = VK_FALSE;
    instancesVk.data.deviceAddress = instanceAddress;

    // Put the above into a VkAccelerationStructureGeometryKHR. We need to put the
    // instances struct in a union and label it as instance data.
    VkAccelerationStructureGeometryKHR topASGeometry{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
    topASGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    topASGeometry.geometry.instances = instancesVk;

    // Find sizes
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
    buildInfo.flags = flags;
    buildInfo.mode = /*update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :*/ VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &topASGeometry;

    uint32_t count = (uint32_t)createInfo.blasInstances.size();
    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
    vkGetAccelerationStructureBuildSizesKHR(
        GetRenderDevice().GetDevice(), 
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo, &count, &sizeInfo
    );

    // Create TLAS:
    VkAccelerationStructureCreateInfoKHR acclCreateInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR };
    acclCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    acclCreateInfo.size = sizeInfo.accelerationStructureSize;

    BufferHandle tlasBuffer;
    VkAccelerationStructureKHR accl = CreateAcceleration(acclCreateInfo, &tlasBuffer);
    TlasHandle ret(objectsPool.tlases.Allocate(*this, createInfo, accl, tlasBuffer, instanceBuffer));
    acclBuilder.StageTlasBuilding(ret, buildInfo, flags);
    return ret;
}

VkAccelerationStructureKHR Renderer::RenderBackend::CreateAcceleration(VkAccelerationStructureCreateInfoKHR& info, BufferHandle* buffer)
{
    BufferCreateInfo bufferInfo;
    bufferInfo.size = info.size;
    bufferInfo.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_STORAGE;
    bufferInfo.domain = MemoryUsage::GPU_ONLY;
    *buffer = this->CreateBuffer(bufferInfo);
    info.buffer = (*buffer)->GetApiObject();

    VkAccelerationStructureKHR accl;
    CALL_VK(vkCreateAccelerationStructureKHR(renderDevice.GetDevice(), &info, NULL, &accl));
    return accl;
}

void Renderer::RenderBackend::SetSamplerCount(uint32 msaaSamplerCount)
{
    this->msaaSamplerCount = renderDevice.GetUsableSampleCount(msaaSamplerCount);
}

Renderer::FenceHandle Renderer::RenderBackend::RequestFence()
{
    VkFence fence = fenceManager.RequestClearedFence();
    FenceHandle h(objectsPool.fences.Allocate(*this, fence));
    return h;
}

void Renderer::RenderBackend::ResetFence(VkFence fence, bool isWaited)
{
    if (isWaited) {
        vkResetFences(renderDevice.GetDevice(), 1, &fence);
        fenceManager.Recycle(fence);
    } else {
        Frame().recycleFences.EmplaceBack(fence);
        Frame().shouldDestroy = true;
    }
}

Renderer::SemaphoreHandle Renderer::RenderBackend::RequestSemaphore()
{
    VkSemaphore sem = semaphoreManager.RequestSemaphore();
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, sem));
    return ptr;
}

Renderer::PiplineEventHandle Renderer::RenderBackend::RequestPiplineEvent()
{
    VkEvent event = eventManager.RequestEvent();
    PiplineEventHandle ptr(objectsPool.events.Allocate(*this, event));
    return ptr;
}

Renderer::ImageHandle Renderer::RenderBackend::CreateImage(const ImageCreateInfo& createInfo, const void* data)
{
    MemoryUsage memUsage = MemoryUsage::USAGE_UNKNOWN;

    VkImageCreateInfo info;
    info.sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext       = NULL;
    info.flags       = createInfo.flags;
    info.imageType   = createInfo.type;
    info.format      = createInfo.format;
    info.extent      = { createInfo.width, createInfo.height, createInfo.depth };
    info.mipLevels   = createInfo.levels;
    info.arrayLayers = createInfo.layers;
    info.samples     = createInfo.samples;
    info.usage       = createInfo.usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;

    switch(createInfo.domain) {
    case ImageDomain::PHYSICAL:
        memUsage = MemoryUsage::GPU_ONLY;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        break;
    case ImageDomain::TRANSIENT:
        memUsage = MemoryUsage::GPU_ONLY;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        break;
    case ImageDomain::LINEAR_HOST:
        memUsage = MemoryUsage::CPU_ONLY;
        info.tiling = VK_IMAGE_TILING_LINEAR;
        info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        break;
    case ImageDomain::LINEAR_HOST_CACHED:
        memUsage = MemoryUsage::CPU_CACHED;
        info.tiling = VK_IMAGE_TILING_LINEAR;
        info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        break;
    }

    StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;
    info.sharingMode = (VkSharingMode)(createInfo.queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & createInfo.queueFamilies) {
            queueFamilyIndices.AllocateInit(1, renderDevice.GetQueueFamilyIndices().queueFamilies[i]);
        }
    }

    if (info.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        info.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
        info.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    VkImage apiImage;
    if (vkCreateImage(renderDevice.GetDevice(), &info, NULL, &apiImage) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a image!");
    }
    
    MemoryView imageMemory;
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderDevice.GetDevice(), apiImage, &memRequirements);
    uint32 memoryTypeIndex = renderDevice.FindMemoryTypeIndex(memRequirements.memoryTypeBits, memUsage);
    imageMemory = gpuMemoryAllocator.Allocate(memoryTypeIndex, memRequirements.size, memRequirements.alignment);
    vkBindImageMemory(renderDevice.GetDevice(), apiImage, imageMemory.memory, imageMemory.offset);

    ImageHandle ret = ImageHandle(objectsPool.images.Allocate(*this, apiImage, createInfo, imageMemory));

    if (data) {
        if (memUsage == MemoryUsage::GPU_ONLY) {
            stagingManager.Stage(*ret, data, createInfo.width * createInfo.height * FormatToChannelCount(createInfo.format));
        } else {
            // TODO: add uploading directly from CPU
            ASSERTF(true, "Not supported!");
        }
    } else {
        if (createInfo.layout != VK_IMAGE_LAYOUT_UNDEFINED) {
            // TODO: add layout trasnisioning here: using staging manager:
            stagingManager.ChangeImageLayout(*ret, info.initialLayout, createInfo.layout);
        }
    }

    return ret;
}

Renderer::ImageViewHandle Renderer::RenderBackend::CreateImageView(const ImageViewCreateInfo& createInfo)
{
    ImageViewCreateInfo info = createInfo;
    const auto& imageCreateInfo = createInfo.image->GetInfo();

    if (createInfo.format == VK_FORMAT_UNDEFINED) {
        info.format = imageCreateInfo.format;
    }

    if (createInfo.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM) {
        info.viewType = GetImageViewType(imageCreateInfo, &createInfo);
    }

    if (createInfo.levels == VK_REMAINING_MIP_LEVELS) {
        info.levels = imageCreateInfo.levels - createInfo.baseLevel;
    }

    if (createInfo.layers == VK_REMAINING_ARRAY_LAYERS) {
        info.layers = imageCreateInfo.layers - createInfo.baseLayer;
    }

    VkImageViewCreateInfo viewInfo;
    viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext      = NULL;
    viewInfo.flags      = 0;
    viewInfo.image      = info.image->GetApiObject();
    viewInfo.viewType   = info.viewType;
    viewInfo.format     = info.format;
    viewInfo.components = info.swizzle;

    viewInfo.subresourceRange = { 
        FormatToAspectMask(viewInfo.format), 
        info.baseLevel, info.levels, 
        info.baseLayer, info.layers
    };

    VkImageView apiImageView;
    vkCreateImageView(renderDevice.GetDevice(), &viewInfo, NULL, &apiImageView);

    ImageViewHandle ret(objectsPool.imageViews.Allocate(*this, apiImageView, info));
    return ret;
}

bool Renderer::RenderBackend::CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo)
{
    outBuffer = renderDevice.CreateBuffer(createInfo);

    // TODO: fix this!! this is quick and dirty way
    if (!(createInfo.usage & BufferUsage::SHADER_DEVICE_ADDRESS)) {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(renderDevice.GetDevice(), outBuffer, &memRequirements);
        uint32 memoryTypeIndex = renderDevice.FindMemoryTypeIndex(memRequirements.memoryTypeBits, createInfo.domain);
        outMemoryView = gpuMemoryAllocator.Allocate(memoryTypeIndex, createInfo.size, memRequirements.alignment);
        vkBindBufferMemory(renderDevice.GetDevice(), outBuffer, outMemoryView.memory, outMemoryView.offset);
    } else {
        outMemoryView.offset = 0;
        outMemoryView.size = createInfo.size;
        outMemoryView.padding = 0;
        outMemoryView.mappedData = NULL;
        outMemoryView.alignment = 0;

        outMemoryView.memory = renderDevice.CreateBufferMemory(createInfo, outBuffer);

        if (createInfo.domain == MemoryUsage::CPU_ONLY || createInfo.domain == MemoryUsage::CPU_CACHED || createInfo.domain == MemoryUsage::CPU_COHERENT) {
            vkMapMemory(renderDevice.GetDevice(), outMemoryView.memory, 0, createInfo.size, 0, &outMemoryView.mappedData);
        }
    }

    return true;
}

Renderer::BufferHandle Renderer::RenderBackend::CreateBuffer(const BufferInfo& createInfo, const void* data)
{
    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    this->CreateBufferInternal(apiBuffer, bufferMemory, createInfo);
    BufferHandle ret(objectsPool.buffers.Allocate(*this, apiBuffer, createInfo, bufferMemory));

    if (data) {
        if (createInfo.domain == MemoryUsage::CPU_ONLY || createInfo.domain == MemoryUsage::CPU_CACHED || createInfo.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }

    return ret;
}

Renderer::RingBufferHandle Renderer::RenderBackend::CreateRingBuffer(const BufferInfo& createInfo, const uint32 ringSize, const void* data)
{
    BufferInfo info = createInfo;
    const DeviceSize alignment = this->renderDevice.internal.gpuProperties.limits.minUniformBufferOffsetAlignment;
    const DeviceSize padding = (alignment - (info.size % alignment)) % alignment;
    const DeviceSize alignedSize = info.size + padding;
    info.size = alignedSize * ringSize; //- padding; // here we must remove padding as we dont need it at the end but (otherwise waste of memory)
                                                     // this is going to complicate our calulations later so better keep it
    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    // Removing padding from total size, as we dont need the last bytes for alignement
    // alignedSize * NUM_FRAMES - padding, data, usage, memoryUsage, queueFamilies
    this->CreateBufferInternal(apiBuffer, bufferMemory, info);
    RingBufferHandle ret(objectsPool.ringBuffers.Allocate(*this, apiBuffer, info, bufferMemory, (uint32)alignedSize, ringSize));

    if (data) {
        if (info.domain == MemoryUsage::CPU_ONLY || info.domain == MemoryUsage::CPU_CACHED || info.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }

    return ret;
}

Renderer::SamplerHandle Renderer::RenderBackend::CreateSampler(const SamplerInfo& createInfo)
{
    VkSampler sampler;
    VkSamplerCreateInfo info;
    SamplerInfo::FillVkSamplerCreateInfo(createInfo, info);
    vkCreateSampler(renderDevice.GetDevice(), &info, NULL, &sampler);
    SamplerHandle ret(objectsPool.samplers.Allocate(*this, sampler, createInfo));
    return ret;
}

Renderer::CommandBufferHandle Renderer::RenderBackend::RequestCommandBuffer(CommandBuffer::Type type)
{
    uint32 typeIndex = (uint32)type;
    if (type == CommandBuffer::Type::RAY_TRACING)
        typeIndex = (uint32)CommandBuffer::Type::GENERIC;

    PerFrame& frame = Frame();
    VkCommandBuffer buffer = frame.commandPools[0][typeIndex].RequestCommandBuffer();
    CommandBufferHandle handle(objectsPool.commandBuffers.Allocate(this, buffer, (CommandBuffer::Type)typeIndex));
    handle->Begin();
    return handle;
}

Renderer::SemaphoreHandle Renderer::RenderBackend::GetImageAcquiredSemaphore()
{
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, renderContext.GetImageAcquiredSemaphore()));
    ptr->SetNoClean();
    return ptr;
}

Renderer::SemaphoreHandle Renderer::RenderBackend::GetDrawCompletedSemaphore()
{
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, renderContext.GetDrawCompletedSemaphore()));
    ptr->SetNoClean();
    return ptr;
}

Renderer::DescriptorSetAllocator* Renderer::RenderBackend::RequestDescriptorSetAllocator(const DescriptorSetLayout& layout)
{
    Hasher h;
    h.Data(reinterpret_cast<const uint32*>(layout.GetDescriptorSetLayoutBindings()), sizeof(VkDescriptorSetLayoutBinding) * layout.GetBindingsCount());

    // For the weird return value check: https://en.cppreference.com/w/cpp/container/unordered_map/emplace
    const auto ctor_arg = std::pair<RenderDevice*, const DescriptorSetLayout&>(&renderDevice, layout);
    const auto& res = descriptorSetAllocators.emplace(h.Get(), ctor_arg);

    if (res.second) {
        res.first->second.Init();
    }

    return &res.first->second;
}

Renderer::Pipeline& Renderer::RenderBackend::RequestPipeline(ShaderProgram& program, const RenderPass& rp, const GraphicsState& state)
{
   return pipelineAllocator.RequestPipline(program, rp, state);
}

Renderer::Pipeline& Renderer::RenderBackend::RequestPipeline(ShaderProgram& program)
{
    return pipelineAllocator.RequestPipline(program);
}

void Renderer::RenderBackend::CreateShaderProgram(const std::initializer_list<ShaderProgram::ShaderStage>& shaderStages, ShaderProgram* shaderProgramOut)
{
    // shaderProgramOut->Create(*this, shaderStages);
}

const Renderer::RenderPass& Renderer::RenderBackend::RequestRenderPass(const RenderPassInfo& info, bool compatible)
{
    Hasher h;
    VkFormat formats[MAX_ATTACHMENTS];
    VkFormat depthStencilFormat;
    uint32 lazy = 0;
    uint32 optimal = 0;

    for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
        ASSERT(!info.colorAttachments[i]);
        formats[i] = info.colorAttachments[i]->GetInfo().format;

        if (info.colorAttachments[i]->GetImage()->GetInfo().domain == ImageDomain::TRANSIENT) {
            lazy |= 1u << i;
        }

        if (info.colorAttachments[i]->GetImage()->GetInfo().layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            optimal |= 1u << i;
        }
        
        // This can change external subpass dependencies, so it must always be hashed.
        h.u32(info.colorAttachments[i]->GetImage()->GetSwapchainLayout());
    }

    if (info.depthStencil) {
        if (info.depthStencil->GetImage()->GetInfo().domain == ImageDomain::TRANSIENT)
            lazy |= 1u << info.colorAttachmentCount;
        if (info.depthStencil->GetImage()->GetInfo().layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            optimal |= 1u << info.colorAttachmentCount;
    }

    h.u32(info.baseLayer);
    h.u32(info.layersCount);
    h.u32(info.subpassesCount);

    for (unsigned i = 0; i < info.subpassesCount; i++) {
        h.u32(info.subpasses[i].colorAttachmentsCount);
        h.u32(info.subpasses[i].inputAttachmentsCount);
        h.u32(info.subpasses[i].resolveAttachmentsCount);
        h.u32(static_cast<uint32_t>(info.subpasses[i].depthStencilMode));

        for (unsigned j = 0; j < info.subpasses[i].colorAttachmentsCount; j++)
            h.u32(info.subpasses[i].colorAttachments[j]);
        for (unsigned j = 0; j < info.subpasses[i].inputAttachmentsCount; j++)
            h.u32(info.subpasses[i].inputAttachments[j]);
        for (unsigned j = 0; j < info.subpasses[i].resolveAttachmentsCount; j++)
            h.u32(info.subpasses[i].resolveAttachments[j]);
    }

    depthStencilFormat = info.depthStencil ? info.depthStencil->GetInfo().format : VK_FORMAT_UNDEFINED;
    h.Data(formats, info.colorAttachmentCount * sizeof(VkFormat));
    h.u32(info.colorAttachmentCount);
    h.u32(depthStencilFormat);

    // Compatible render passes do not care about load/store, or image layouts.
    if (!compatible) {
        h.u32(info.opFlags);
        h.u32(info.clearAttachments);
        h.u32(info.loadAttachments);
        h.u32(info.storeAttachments);
        h.u32(optimal);
    }

    // Lazy flag can change external subpass dependencies, which is not compatible.
    h.u32(lazy);

    Hash hash = h.Get();
    auto rp = renderPasses.find(hash);

    if (rp != renderPasses.end()) {
        return rp->second;
    }

    auto rp2 = renderPasses.emplace(hash, RenderPass(renderDevice, info));
    // printf("Creating render pass ID: %llu.\n", hash);
    rp2.first->second.hash = h.Get();
    return rp2.first->second;
}

const Renderer::Framebuffer& Renderer::RenderBackend::RequestFramebuffer(const RenderPassInfo& info, const RenderPass* rp)
{
    if (!rp) {
        rp = &this->RequestRenderPass(info);
    }

    return framebufferAllocator.RequestFramebuffer(*rp, info);
}

Renderer::RenderPassInfo Renderer::RenderBackend::GetSwapchainRenderPass(SwapchainRenderPass style)
{
    RenderPassInfo info;
    info.colorAttachmentCount = 1;
    info.colorAttachments[0] = renderContext.GetSwapchain().GetSwapchainImage(renderContext.GetCurrentImageIndex())->GetView();
    info.clearColor[0] = { 0.051f, 0.051f, 0.051f, 0.0f };
    info.clearAttachments = 1u << 0;
    info.storeAttachments = 1u << 0;

    switch (style) {
    case SwapchainRenderPass::DEPTH:
    {
        info.opFlags |= RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT;
        info.depthStencil =
            &GetTransientAttachment(renderContext.GetSwapchain().GetExtent().width,
                renderContext.GetSwapchain().GetExtent().height, renderContext.GetSwapchain().FindSupportedDepthFormat(), 0, msaaSamplerCount);
        break;
    }

    case SwapchainRenderPass::DEPTH_STENCIL:
    {
        info.opFlags |= RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT;
        info.depthStencil =
            &GetTransientAttachment(renderContext.GetSwapchain().GetExtent().width,
                renderContext.GetSwapchain().GetExtent().height, renderContext.GetSwapchain().FindSupportedDepthStencilFormat(), 0, msaaSamplerCount);
        break;
    }
    default:
        break;
    }

    return info;
}

Renderer::ImageView& Renderer::RenderBackend::GetTransientAttachment(uint32 width, uint32 height, VkFormat format, uint32 index, uint32 samples, uint32 layers)
{
    return transientAttachmentAllocator.RequestAttachment(width, height, format, index, samples, layers);
}

void Renderer::RenderBackend::DestroyPendingObjects(PerFrame& frame)
{
    if (!frame.shouldDestroy)
        return;

    VkDevice dev = renderDevice.GetDevice();

    for (auto& rp : frame.destroyedRenderPasses)
        vkDestroyRenderPass(dev, rp, NULL);

    for (auto& dsc : frame.destroyedDescriptorPool)
        vkDestroyDescriptorPool(dev, dsc, NULL);

    for (auto& fb : frame.destroyedFramebuffers)
        vkDestroyFramebuffer(dev, fb, NULL);

    for (auto& view : frame.destroyedImageViews)
        vkDestroyImageView(dev, view, NULL);

    for (auto& img : frame.destroyedImages)
        vkDestroyImage(dev, img, NULL);

    for (auto& view : frame.destroyedBufferViews)
        vkDestroyBufferView(dev, view, NULL);

    for (auto& buff : frame.destroyedBuffers)
        vkDestroyBuffer(dev, buff, NULL);

    for (auto& sem : frame.destroyedSemaphores)
        vkDestroySemaphore(dev, sem, NULL);

    for (auto& sampler : frame.destroyedSamplers)
        vkDestroySampler(dev, sampler, NULL);

    if (enabledFeatures & RAY_TRACING) {
        for (auto& accl : frame.destroyedAccls)
            vkDestroyAccelerationStructureKHR(dev, accl, NULL);

        frame.destroyedAccls.Clear();
    }

    // Free memory:
    for (auto& mem : frame.freedMemory)
        vkFreeMemory(dev, mem, NULL);

    // Recycle:
    for (auto& sem : frame.recycleSemaphores)
        semaphoreManager.Recycle(sem);

    // vkResetFences(dev, (uint32)frame.recycleFences.Size(), frame.recycleFences.Data());
    for (auto& fence : frame.recycleFences)
        fenceManager.Recycle(fence);

    frame.destroyedFramebuffers.Clear();
    frame.destroyedImageViews.Clear();
    frame.destroyedImages.Clear();
    frame.freedMemory.Clear();
    frame.destroyedSemaphores.Clear();
    frame.recycleSemaphores.Clear();
    frame.recycleFences.Clear();

    frame.destroyedSamplers.Clear();
    frame.destroyedBuffers.Clear();
    frame.destroyedBufferViews.Clear();
    frame.destroyedRenderPasses.Clear();
    frame.destroyedDescriptorPool.Clear();
    frame.shouldDestroy = false;
}

void Renderer::RenderBackend::DestroyImage(VkImage image)
{
    PerFrame& frame = this->Frame();
    frame.destroyedImages.EmplaceBack(image);
    frame.shouldDestroy = true;
}

void Renderer::RenderBackend::DestroyImageView(VkImageView view)
{
    PerFrame& frame = this->Frame();
    frame.destroyedImageViews.EmplaceBack(view);
    frame.shouldDestroy = true;
}

void Renderer::RenderBackend::DestroyFramebuffer(VkFramebuffer fb)
{
    PerFrame& frame = this->Frame();
    frame.destroyedFramebuffers.EmplaceBack(fb);
    frame.shouldDestroy = true;
}

void Renderer::RenderBackend::FreeMemory(VkDeviceMemory memory)
{
    PerFrame& frame = this->Frame();
    frame.freedMemory.EmplaceBack(memory);
    frame.shouldDestroy = true;
}

void Renderer::RenderBackend::RecycleSemaphore(VkSemaphore sem)
{
    Frame().recycleSemaphores.EmplaceBack(sem);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroySemaphore(VkSemaphore sem)
{
    Frame().destroyedSemaphores.EmplaceBack(sem);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroryEvent(VkEvent event)
{
    Frame().destroyedEvents.EmplaceBack(event);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroyBuffer(VkBuffer buffer)
{
    Frame().destroyedBuffers.EmplaceBack(buffer);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroyBufferView(VkBufferView view)
{
    Frame().destroyedBufferViews.EmplaceBack(view);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroySampler(VkSampler sampler)
{
    Frame().destroyedSamplers.EmplaceBack(sampler);
    Frame().shouldDestroy = true;
}

void Renderer::RenderBackend::DestroyAllFrames()
{
    for (PerFrame& frame : perFrame) {
        frame.shouldDestroy = true;

        for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
            frame.commandPools[0][i].Destroy();
            frame.submissions[i].Clear();
        }

        this->DestroyPendingObjects(frame);
    }

    objectsPool.commandBuffers.Clear();
    objectsPool.buffers.Clear();
    objectsPool.ringBuffers.Clear();
    objectsPool.images.Clear();
    objectsPool.imageViews.Clear();
    objectsPool.samplers.Clear();
    objectsPool.fences.Clear();
    objectsPool.semaphores.Clear();
    objectsPool.events.Clear();

    // RT:
    if (enabledFeatures & RAY_TRACING) {
        objectsPool.blases.Clear();
        objectsPool.tlases.Clear();
    }
}

TRE_NS_END
