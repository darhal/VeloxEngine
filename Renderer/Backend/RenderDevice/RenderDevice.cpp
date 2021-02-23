#include "RenderDevice.hpp"
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <unordered_set>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

Renderer::RenderDevice::RenderDevice(RenderContext* ctx) :
    internal{ 0 },
    renderContext(ctx),
    stagingManager{ *this },
    acclBuilder(*this),
    framebufferAllocator(this),
    transientAttachmentAllocator(*this, true),
    pipelineAllocator(*this),
    submitSwapchain(false)
{

}

Renderer::RenderDevice::~RenderDevice()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////              Basic functionality:            //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32 Renderer::RenderDevice::CreateRenderDevice(const RenderInstance& renderInstance,
    const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    RenderContext& ctx = *renderContext;

    internal.gpu = PickGPU(renderInstance, ctx);
    ASSERTF(internal.gpu == VK_NULL_HANDLE, "Couldn't pick a GPU.");

    this->FetchDeviceAvailableExtensions();
    vkGetPhysicalDeviceFeatures(internal.gpu, &internal.gpuFeatures);
    vkGetPhysicalDeviceProperties(internal.gpu, &internal.gpuProperties);

    internal.queueFamilyIndices     = FindQueueFamilies(internal.gpu, ctx.GetSurface());
    internal.isPresentQueueSeprate  = internal.queueFamilyIndices.queueFamilies[Internal::QFT_GRAPHICS] != 
                                            internal.queueFamilyIndices.queueFamilies[Internal::QFT_PRESENT];
    internal.isTransferQueueSeprate = internal.queueFamilyIndices.queueFamilies[Internal::QFT_GRAPHICS] !=
                                            internal.queueFamilyIndices.queueFamilies[Internal::QFT_TRANSFER];

    vkGetPhysicalDeviceMemoryProperties(internal.gpu, &internal.memoryProperties);

    internal.rtProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    internal.gpuProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    internal.gpuProperties2.pNext = &internal.rtProperties;
    vkGetPhysicalDeviceProperties2(internal.gpu, &internal.gpuProperties2);
    return CreateLogicalDevice(renderInstance, extensions, extCount, layers, layerCount);
}

void Renderer::RenderDevice::DestroryRenderDevice()
{
    vkDestroyDevice(internal.device, NULL);
}

int32 Renderer::RenderDevice::CreateLogicalDevice(const RenderInstance& renderInstance,
    const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    ASSERT(renderInstance.GetApiObject() == VK_NULL_HANDLE);

    StaticVector<const char*> extensionsArr;
    StaticVector<const char*> layersArr;

    for (const auto& ext : VK_REQ_DEVICE_EXT) {
        Hash h = Utils::Data(ext, strlen(ext));

        if (availbleDevExtensions.find(h) == availbleDevExtensions.end()) {
            TRE_LOGE("Can't load mandatory extension '%s' not supported by device", ext);
            return -1;
        }

        extensionsArr.PushBack(ext);
        deviceExtensions.emplace(h);
    }

    for (uint32 i = 0; i < extCount; i++) {
        Hash h = Utils::Data(extensions[i], strlen(extensions[i]));

        if (availbleDevExtensions.find(h) == availbleDevExtensions.end()) {
            TRE_LOGW("Skipping extension '%s' not supported by device", extensions[i]);
            continue;
        }

        extensionsArr.PushBack(extensions[i]);
        deviceExtensions.emplace(h);
    }

    printf("Device Ext:\n");
    for (auto c : extensionsArr)
        printf("\t%s\n", c);

    for (uint32 i = 0; i < layerCount; i++) {
        layersArr.PushBack(layers[i]);
    }

    printf("Device Layers:\n");
    for (auto c : layersArr)
        printf("\t%s\n", c);

    VkDevice device = VK_NULL_HANDLE;
    float queuePriority = 1.0f;
    const Internal::QueueFamilyIndices& indices = internal.queueFamilyIndices;

    TRE::Vector<VkDeviceQueueCreateInfo> queueCreateInfos(Internal::QFT_MAX);
    std::unordered_set<uint32> uniqueQueueFamilies(std::begin(indices.queueFamilies), std::end(indices.queueFamilies));

    for (uint32 queueFamily : uniqueQueueFamilies) {
        if (queueFamily != UINT32_MAX) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.EmplaceBack(queueCreateInfo);
        }
    }

    // Enable some device features:
    // TODO: check if the GPU supports this feature
    internal.accelFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    internal.rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    internal.buffAdrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;

    internal.deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    internal.deviceFeatures2.pNext = &internal.accelFeatures;
    internal.accelFeatures.pNext = &internal.rtPipelineFeatures;
    internal.rtPipelineFeatures.pNext = &internal.buffAdrFeatures;
    vkGetPhysicalDeviceFeatures2(internal.gpu, &internal.deviceFeatures2);

    //deviceFeatures2.features.samplerAnisotropy = VK_TRUE;
    //deviceFeatures2.features.fillModeNonSolid = VK_TRUE;
    //buffAdrFeatures.bufferDeviceAddress = VK_TRUE;
    //deviceFeatures2.features.robustBufferAccess = VK_FALSE;
    
    // Classical VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo;
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                    = &internal.deviceFeatures2;
    createInfo.flags                    = 0;
    createInfo.pEnabledFeatures         = NULL;//&deviceFeatures2.features;

    createInfo.pQueueCreateInfos        = queueCreateInfos.Data();
    createInfo.queueCreateInfoCount     = (uint32)queueCreateInfos.Size();

    createInfo.enabledExtensionCount    = (uint32)extensionsArr.Size();
    createInfo.ppEnabledExtensionNames  = extensionsArr.begin();

    createInfo.enabledLayerCount        = (uint32)layersArr.Size();
    createInfo.ppEnabledLayerNames      = layersArr.begin();

    VkResult res = vkCreateDevice(internal.gpu, &createInfo, NULL, &internal.device);
    load_VK_EXTENSION_SUBSET(renderInstance.GetApiObject(), vkGetInstanceProcAddr, internal.device, vkGetDeviceProcAddr);

    if (res != VK_SUCCESS) {
        ASSERTF(true, "Couldn't create a logical device (%s)!", GetVulkanResultString(res));
        return -1;
    }

    for (uint32 queueIndex = 0; queueIndex < Internal::QFT_MAX; queueIndex++) {
        if (indices.queueFamilies[queueIndex] != UINT32_MAX) {
            vkGetDeviceQueue(internal.device, indices.queueFamilies[queueIndex], 0, &internal.queues[queueIndex]);
        }
    }

    return 0;
}

void Renderer::RenderDevice::Init(uint32 enabledFeatures)
{
    this->enabledFeatures = enabledFeatures;
    const Internal::QueueFamilyIndices& queueFamilyIndices = this->GetQueueFamilyIndices();

    for (uint32 f = 0; f < renderContext->GetNumFrames(); f++) {
        for (uint32 t = 0; t < MAX_THREADS; t++) {
            for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
                if (queueFamilyIndices.queueFamilies[i] == UINT32_MAX) {
                    TRE_LOGW("Skipping queue familly index %d", i);
                    continue;
                }

                PerFrame& frame = perFrame[f];
                new (&frame.commandPools[t][i]) CommandPool(this, queueFamilyIndices.queueFamilies[i], (CommandBuffer::Type)i);
            }
        }
    }

    gpuMemoryAllocator.Init(internal);
    fenceManager.Init(this);
    semaphoreManager.Init(this);
    eventManager.Init(this);
    stagingManager.Init();

    // RT:
    if (enabledFeatures & RAY_TRACING)
        acclBuilder.Init();
}

VkDeviceMemory Renderer::RenderDevice::AllocateDedicatedMemory(VkImage image, MemoryUsage memoryDomain) const
{
    VkDeviceMemory memory;
    VkMemoryAllocateInfo info;
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(internal.device, image, &memRequirements);

    info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext           = NULL;
    info.allocationSize  = memRequirements.size;
    info.memoryTypeIndex = this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, memoryDomain);

    vkAllocateMemory(internal.device, &info, NULL, &memory);
    vkBindImageMemory(internal.device, image, memory, 0);
    return memory;
}

VkDeviceMemory Renderer::RenderDevice::AllocateDedicatedMemory(VkBuffer buffer, MemoryUsage memoryDomain) const
{
    VkDeviceMemory memory;
    VkMemoryAllocateInfo info;
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(internal.device, buffer, &memRequirements);

    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = NULL;
    info.allocationSize = memRequirements.size;
    info.memoryTypeIndex = this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, memoryDomain);

    vkAllocateMemory(internal.device, &info, NULL, &memory);
    vkBindBufferMemory(internal.device, buffer, memory, 0);
    return memory;
}

void Renderer::RenderDevice::FreeDedicatedMemory(VkDeviceMemory memory) const
{
    vkFreeMemory(internal.device, memory, NULL);
}

VkSampleCountFlagBits Renderer::RenderDevice::GetUsableSampleCount(uint32 sampleCount) const
{
    ASSERT(~(sampleCount - 1) == 0);

    VkSampleCountFlags counts = internal.gpuProperties.limits.framebufferColorSampleCounts& internal.gpuProperties.limits.framebufferDepthSampleCounts;

    while (!(counts & sampleCount)) {
        sampleCount >>= 1;
    }

    return VkSampleCountFlagBits(sampleCount);
}

VkSampleCountFlagBits Renderer::RenderDevice::GetMaxUsableSampleCount() const
{
    VkSampleCountFlags counts = internal.gpuProperties.limits.framebufferColorSampleCounts & internal.gpuProperties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

uint32 Renderer::RenderDevice::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const
{
    const VkPhysicalDeviceMemoryProperties& memProperties = this->GetMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    ASSERTF(true, "Failed to find suitable memory type!");
    return ~0u;
}

uint32 Renderer::RenderDevice::FindMemoryTypeIndex(uint32 typeFilter, MemoryUsage usage) const
{
    VkMemoryPropertyFlags required = 0;
    // VkMemoryPropertyFlags preferred = 0;

    switch (usage) {
    case MemoryUsage::GPU_ONLY:
        required |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        // preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case MemoryUsage::LINKED_GPU_CPU:
        required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case MemoryUsage::CPU_ONLY:
        required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        // preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case MemoryUsage::CPU_CACHED:
        required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        // preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;
    case MemoryUsage::CPU_COHERENT:
        required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        // preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;
    default:
        ASSERTF(true, "Unknown memory usage!");
    }

    return FindMemoryType(typeFilter, required);
}

VkBuffer Renderer::RenderDevice::CreateBufferHelper(const BufferInfo& info) const
{
    StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;

    VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = info.size;
    bufferInfo.usage = info.usage;
    bufferInfo.sharingMode = (VkSharingMode)SharingMode::EXCLUSIVE;

    if (info.domain == MemoryUsage::GPU_ONLY) {
        bufferInfo.usage |= BufferUsage::TRANSFER_DST;
    }

    if (info.queueFamilies) {
        for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
            if (Internal::QUEUE_FAMILY_FLAGS[i] & info.queueFamilies) {
                queueFamilyIndices.AllocateInit(1, this->GetQueueFamilyIndices().queueFamilies[i]);
            }
        }

        bufferInfo.sharingMode = (VkSharingMode)SharingMode::CONCURRENT;
        bufferInfo.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
        bufferInfo.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    VkBuffer outBuffer;
    CALL_VK(vkCreateBuffer(this->GetDevice(), &bufferInfo, NULL, &outBuffer));
    return outBuffer;
}

VkDeviceMemory Renderer::RenderDevice::CreateBufferMemory(const BufferInfo& info, VkBuffer buffer, VkDeviceSize* alignedSize, uint32 multiplier) const
{
    VkMemoryRequirements2 memoryReqs;
    VkMemoryDedicatedRequirements   dedicatedRegs{ VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS };
    VkBufferMemoryRequirementsInfo2 bufferReqs{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 };
    memoryReqs.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
    memoryReqs.pNext = &dedicatedRegs;
    bufferReqs.buffer = buffer;
    vkGetBufferMemoryRequirements2(this->GetDevice(), &bufferReqs, &memoryReqs);

    const VkDeviceSize alignMod = memoryReqs.memoryRequirements.size % memoryReqs.memoryRequirements.alignment;
    const VkDeviceSize alignedSizeConst =
        (alignMod == 0) ?
        memoryReqs.memoryRequirements.size :
        (memoryReqs.memoryRequirements.size + memoryReqs.memoryRequirements.alignment - alignMod);

    if (alignedSize)
        *alignedSize = alignedSizeConst;

    VkMemoryAllocateFlagsInfo memFlagInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO };
    if (info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        memFlagInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memFlagInfo;
    memoryAllocateInfo.allocationSize = (multiplier == 1) ? memoryReqs.memoryRequirements.size : alignedSizeConst * multiplier;
    memoryAllocateInfo.memoryTypeIndex = this->FindMemoryTypeIndex(memoryReqs.memoryRequirements.memoryTypeBits, info.domain);

    VkDeviceMemory mem;
    CALL_VK(vkAllocateMemory(this->GetDevice(), &memoryAllocateInfo, NULL, &mem));

    if (multiplier == 1) {
        vkBindBufferMemory(this->GetDevice(), buffer, mem, 0);
    }

    return mem;
}

VkAccelerationStructureKHR Renderer::RenderDevice::CreateAcceleration(VkAccelerationStructureCreateInfoKHR& info, VkBuffer* buffer) const
{
    BufferCreateInfo bufferInfo;
    bufferInfo.size = info.size;
    bufferInfo.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_STORAGE;
    bufferInfo.domain = MemoryUsage::GPU_ONLY;
    *buffer = this->CreateBufferHelper(bufferInfo);
    this->CreateBufferMemory(bufferInfo, *buffer);
    info.buffer = *buffer;

    VkAccelerationStructureKHR accl;
    CALL_VK(vkCreateAccelerationStructureKHR(this->GetDevice(), &info, NULL, &accl));
    return accl;
}

VkCommandBuffer Renderer::RenderDevice::CreateCmdBuffer(VkCommandPool pool, VkCommandBufferLevel level, VkCommandBufferUsageFlags flag) const
{
    // Command buffer allocation:
    VkCommandBuffer cmd;
    VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocInfo.level = level;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(GetDevice(), &allocInfo, &cmd);

    // Begin recording
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flag;
    beginInfo.pInheritanceInfo = NULL;
    vkBeginCommandBuffer(cmd, &beginInfo);

    return cmd;
}

VkResult Renderer::RenderDevice::SubmitCmdBuffer(uint32 queueType, VkCommandBuffer* cmdBuff, uint32 cmdCount,
    VkPipelineStageFlags waitStage, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence) const
{
    for (uint32 i = 0; i < cmdCount; i++) {
        vkEndCommandBuffer(cmdBuff[i]);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = cmdCount;
    submitInfo.pCommandBuffers = cmdBuff;


    if (waitSemaphore != VK_NULL_HANDLE) {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
    }

    if (signalSemaphore != VK_NULL_HANDLE) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;
    }

    return vkQueueSubmit(GetQueue(queueType), 1, &submitInfo, fence);
}

VkDeviceAddress Renderer::RenderDevice::GetBufferAddress(BufferHandle buff) const
{
    VkBufferDeviceAddressInfo bufferAdrInfo;
    bufferAdrInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferAdrInfo.pNext = NULL;
    bufferAdrInfo.buffer = buff->GetApiObject();
    return vkGetBufferDeviceAddressKHR(this->GetDevice(), &bufferAdrInfo);
}

VkPhysicalDevice Renderer::RenderDevice::PickGPU(const RenderInstance& renderInstance,
                                                 const RenderContext& ctx, FPN_RankGPU p_pick_func)
{
    ASSERT(ctx.GetSurface() == NULL);
    ASSERT(renderInstance.GetApiObject() == NULL);

    VkInstance instance = renderInstance.GetApiObject();
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32 deviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "Failed to find GPUs with Vulkan support!\n");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount, VK_NULL_HANDLE);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices) {
        if (p_pick_func(device, ctx.GetSurface())) {
            physicalDevice = device;
            break;
        }
    }

    return physicalDevice;
}

void Renderer::RenderDevice::FetchDeviceAvailableExtensions()
{
    uint32 extensionsCount = 256;
    StaticVector<VkExtensionProperties, 256> extensionsAvailble;
    vkEnumerateDeviceExtensionProperties(internal.gpu, nullptr, &extensionsCount, extensionsAvailble.begin());
    extensionsAvailble.Resize(extensionsCount);
    
    for (const auto& ext : extensionsAvailble) {
        availbleDevExtensions.emplace(Utils::Data(ext.extensionName, strlen(ext.extensionName)));
    }
}

bool Renderer::RenderDevice::IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    Internal::QueueFamilyIndices indices = FindQueueFamilies(gpu, surface);
    
    bool swapChainAdequate = false;
    Renderer::Swapchain::SwapchainSupportDetails swapChainSupport = Renderer::Swapchain::QuerySwapchainSupport(gpu, surface);
    swapChainAdequate = !swapChainSupport.formats.IsEmpty() && !swapChainSupport.presentModes.IsEmpty();

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);
    VkPhysicalDeviceProperties devProp;
    vkGetPhysicalDeviceProperties(gpu, &devProp);
    bool isDiscrete = 1; // devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	return indices.IsComplete() && swapChainAdequate && supportedFeatures.samplerAnisotropy && isDiscrete;
}

Renderer::Internal::QueueFamilyIndices Renderer::RenderDevice::FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface)
{
    Internal::QueueFamilyIndices indices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p_gpu, &queueFamilyCount, NULL);

    TRE::Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount, {});
    vkGetPhysicalDeviceQueueFamilyProperties(p_gpu, &queueFamilyCount, queueFamilies.Data());

    int32 i = 0;

    for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
        if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)) 
        {
            indices.queueFamilies[Internal::QFT_GRAPHICS] = i;
            // indices.queueFamilies[Internal::QFT_RAY_TRACING] = i;
        }

        if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) 
        {
            indices.queueFamilies[Internal::QFT_COMPUTE] = i;
        }

        if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && 
            !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) 
        {
            indices.queueFamilies[Internal::QFT_TRANSFER] = i;
        }

        VkBool32 presentSupport = false;

        if (p_surface) {
            vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu, i, p_surface, &presentSupport);

            if (presentSupport && indices.queueFamilies[Internal::QFT_PRESENT] == UINT32_MAX) {
                indices.queueFamilies[Internal::QFT_PRESENT] = i;
            }
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    if (indices.queueFamilies[Internal::QFT_TRANSFER] == UINT32_MAX) { // falling back to graphics queue
        indices.queueFamilies[Internal::QFT_TRANSFER] = indices.queueFamilies[Internal::QFT_GRAPHICS];
    }

    printf("Complete! (Graphics: %d | Transfer: %d | Compute: %d | Present: %d)\n",
        indices.queueFamilies[Internal::QFT_GRAPHICS],
        indices.queueFamilies[Internal::QFT_TRANSFER],
        indices.queueFamilies[Internal::QFT_COMPUTE],
        indices.queueFamilies[Internal::QFT_PRESENT]
    );
    return indices;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////             Device functionality:            //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Renderer::RenderDevice::Shutdown()
{
    VkDevice device =  this->GetDevice();
    vkDeviceWaitIdle(device);

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

    // Destroy vulkan device:
    this->DestroryRenderDevice();
}

Renderer::CommandBuffer::Type Renderer::RenderDevice::GetPhysicalQueueType(CommandBuffer::Type type)
{
    return type;
}

Renderer::RenderDevice::PerFrame::Submissions& Renderer::RenderDevice::GetQueueSubmissions(CommandBuffer::Type type)
{
    return Frame().submissions[(uint32)type];
}

VkQueue Renderer::RenderDevice::GetQueue(CommandBuffer::Type type)
{
    uint32 typeIndex = (uint32)(type);
    return GetQueue(typeIndex);
}

void Renderer::RenderDevice::Submit(CommandBufferHandle cmd, FenceHandle* fence, uint32 semaphoreCount,
    SemaphoreHandle** semaphores, uint32 signalValuesCount, const uint64* signalValues)
{
    this->Submit(cmd->GetType(), cmd, fence, semaphoreCount, semaphores, signalValuesCount, signalValues);
}

void Renderer::RenderDevice::Submit(Renderer::CommandBuffer::Type type, Renderer::CommandBufferHandle cmd, Renderer::FenceHandle* fence,
                                    uint32 semaphoreCount, Renderer::SemaphoreHandle** semaphores, uint32 signalValuesCount,
                                    const uint64* signalValues)
{
    cmd->End();
    auto& submissions = this->GetQueueSubmissions(type);

    if (fence || semaphoreCount) {
        PerFrame::Submission& submission = submissions.EmplaceBack();
        submission.commands.PushBack(cmd);
        uint32 timelineSemaCount = 0;

        // Inject a semaphore to wait for transfer stage. We gurantee that all of the transfers done before begin frame are finished.
        if (type != CommandBuffer::Type::ASYNC_TRANSFER && stagingManager.GetPreviousStagingBuffer().submitted) {
            // printf("Injecting wait sempahore - ");
            this->AddWaitTimelineSemapore(type, stagingManager.GetTimelineSemaphore(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        }

        // TODO: Potential optimisation here we can just push directly the vk semaphores
        // think if this causes bugs (I dont think it can)
        for (uint32 i = 0; i < semaphoreCount; i++) {
            SemaphoreHandle* sem_ptr = semaphores[i];

            if (!(*sem_ptr)) {
                if (semaphoreCount != signalValuesCount) {// they are probably mixed or full binary semaphores
                    *sem_ptr = this->RequestSemaphore();
                }else { // all of them are timeline semaphores
                    *sem_ptr = this->RequestTimelineSemaphore();
                }
            }

            SemaphoreHandle sem = *sem_ptr;

            if (signalValuesCount && sem->GetType() == Semaphore::TIMELINE) { // semaphores are mixed
                if (!signalValues){ // automaticaly determine the counter of the semaphore
                    submission.timelineSemaSignal.PushBack(sem->IncrementTempValue());
                }else{ // if semaphore counter is defined by user
                    sem->tempValue = signalValues[timelineSemaCount];
                    submission.timelineSemaSignal.PushBack(signalValues[timelineSemaCount]);
                }

                timelineSemaCount++;
            }

            submission.signalSemaphores.EmplaceBack(sem);
        }

        submission.fence = fence;
    }else{
        if (!submissions.Size()) {
            submissions.EmplaceBack();

            // Inject a semaphore to wait for transfer stage. We gurantee that all of the transfers done before begin frame are finished.
            if (type != CommandBuffer::Type::ASYNC_TRANSFER && stagingManager.GetPreviousStagingBuffer().submitted) { // Only added when there is a new submission
                // printf("Injecting wait sempahore - ");
                this->AddWaitTimelineSemapore(type, stagingManager.GetTimelineSemaphore(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
            }
        }

        PerFrame::Submission& submission = submissions.Back();
        submission.commands.PushBack(cmd);
    }
}

void Renderer::RenderDevice::FlushQueue(CommandBuffer::Type type, bool triggerSwapchainSwap)
{
    auto& submissions = this->GetQueueSubmissions(type);

    if (!submissions.Size()){
        return;
    }

    struct SubmitDataOffsets
    {
        uint64 commandBufferOffset   = 0;
        uint64 waitSemaphoreOffset   = 0;
        uint64 signalSemaphoreOffset = 0;
    } offsets, oldOffsets;

    StaticVector<VkSubmitInfo>      submits;
    StaticVector<VkCommandBuffer>   cmds;
    StaticVector<VkSemaphore>       waits;
    StaticVector<VkSemaphore>       signals;
    const bool swapchainResize = renderContext->GetSwapchain().ResizeRequested();

    for (uint32 subId = 0; subId < submissions.Size(); subId++) {
        auto& sub = submissions[subId];
        VkSubmitInfo& submit = submits.EmplaceBack();
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        CommandBufferHandle swapchainCommandBuffer;

        for (auto& cmd : sub.commands) {
            if (!submitSwapchain && cmd->UsesSwapchain()) {
                swapchainCommandBuffer = cmd;
                submitSwapchain = true;
            }

            cmds.PushBack(cmd->GetApiObject());
            offsets.commandBufferOffset++;
        }

        for (auto& sem : sub.waitSemaphores) {
            waits.PushBack(sem->GetApiObject());
            offsets.waitSemaphoreOffset++;
        }

        for (auto& sem : sub.signalSemaphores) {
            signals.EmplaceBack(sem->GetApiObject());
            offsets.signalSemaphoreOffset++;
        }

        // The line below is commented because we can deduce the timeline values automatically so the timelineSemaCount can be effectively 0
        // ASSERTF(timelineSemaCount != signalValuesCount, "The count of timeline semaphores to signal is not equal to signalVlauesCount passed as argument");

        if ((swapchainCommandBuffer || triggerSwapchainSwap) && !swapchainResize) {
            //const uint32 frame = renderContext->GetCurrentFrame();

            //if (!stagingManager.GetStage(frame).submitted) {
                VkPipelineStageFlagBits stage = swapchainCommandBuffer ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                    : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                sub.waitStages.PushBack(stage);
                waits.EmplaceBack(renderContext->GetImageAcquiredSemaphore());
                offsets.waitSemaphoreOffset++;
                // printf("Inject swapchain semaphore\n");
            //}

            signals.EmplaceBack(renderContext->GetDrawCompletedSemaphore());
            offsets.signalSemaphoreOffset++;
        }

        const uint32 waitSemaphoreCount   = offsets.waitSemaphoreOffset - oldOffsets.waitSemaphoreOffset;
        const uint32 signalSemaphoreCount = offsets.signalSemaphoreOffset - oldOffsets.signalSemaphoreOffset;
        const uint32 commandBufferCount   = offsets.commandBufferOffset - oldOffsets.commandBufferOffset;
        VkTimelineSemaphoreSubmitInfo timelineSubmitInfo;

        if (sub.timelineSemaWait.Size() || sub.timelineSemaSignal.Size()) {
            timelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
            timelineSubmitInfo.pNext = NULL;
            timelineSubmitInfo.waitSemaphoreValueCount   = waitSemaphoreCount;   // this should be done this way acooring to specs
            timelineSubmitInfo.pWaitSemaphoreValues      = sub.timelineSemaWait.Data();
            timelineSubmitInfo.signalSemaphoreValueCount = signalSemaphoreCount; // because we implicitly inject the swapchain semaphore
            timelineSubmitInfo.pSignalSemaphoreValues    = sub.timelineSemaSignal.Data();

            // THIS WAS JUST FOR DEBUGGING!
            /*for (int ll = 0; ll < waitSemaphoreCount; ll++) {
                if (sub.timelineSemaWait[ll] != 0) {
                    printf("Wait: %p - wait value: %llu\n", waits[ll], sub.timelineSemaWait[ll]);
                    ll++;
                }
            }

            for (int ll = 0; ll < signalSemaphoreCount; ll++) {
                if (sub.timelineSemaSignal[ll] != 0) {
                    printf("Signal: %p - signal value: %llu\n", signals[ll], sub.timelineSemaSignal[ll]);
                    ll++;
                }
            }*/

            submit.pNext = &timelineSubmitInfo;
        }else{
            submit.pNext = NULL;
        }

        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount   = waitSemaphoreCount;
        submit.pWaitSemaphores      = waits.Data() + oldOffsets.waitSemaphoreOffset;
        submit.pWaitDstStageMask    = sub.waitStages.Data();

        submit.signalSemaphoreCount = signalSemaphoreCount;
        submit.pSignalSemaphores    = signals.Data() + oldOffsets.signalSemaphoreOffset;

        submit.commandBufferCount   = commandBufferCount;
        submit.pCommandBuffers      = cmds.Data() + oldOffsets.commandBufferOffset;

        ASSERTF(sub.fence && swapchainCommandBuffer, "Can't submit command buffers that draw to the swapchain with a fance");
        VkFence vkFence = VK_NULL_HANDLE;

        if (sub.fence) {
            *sub.fence = this->RequestFence();
            vkFence = (*sub.fence)->GetApiObject();
        }

        if (swapchainCommandBuffer || triggerSwapchainSwap) {
            vkFence = renderContext->GetFrameFence();
        }

        if (vkFence || (subId == submissions.Size() - 1)) { // if there is a fence or last submit then vkSubmit
            // printf("Submit commands: %d|%d|%d\n", (bool)swapchainCommandBuffer, lastSubmit, swapchainResize);
            vkQueueSubmit(this->GetQueue(type), submits.Size(), submits.Data(), vkFence);
            submits.Clear();
            cmds.Clear();
            waits.Clear();
            signals.Clear();
            oldOffsets = {0};
        }

        offsets = oldOffsets;
        sub.Clear();
    }

    // Everything flushed, no dtor is called here as we cleared all the elments before
    submissions.Resize(0);
}


void Renderer::RenderDevice::AddWaitSemapore(CommandBuffer::Type type, SemaphoreHandle semaphore, VkPipelineStageFlags stages, bool flush)
{
    ASSERT(stages == 0);

    if (flush) {
        this->FlushQueue(type);
    }

    auto& data = GetQueueSubmissions(type).Back();
    data.waitSemaphores.PushBack(semaphore);
    data.waitStages.PushBack(stages);

    if (semaphore->GetType() == Semaphore::TIMELINE) {
        data.timelineSemaWait.PushBack(semaphore->GetTempValue());
    }
}

void Renderer::RenderDevice::AddWaitTimelineSemapore(Renderer::CommandBuffer::Type type, Renderer::SemaphoreHandle semaphore,
                                                     VkPipelineStageFlags stages, uint64 waitValue, bool flush)
{
    ASSERT(stages == 0);

    if (flush) {
        this->FlushQueue(type);
    }

    auto& data = GetQueueSubmissions(type).Back();
    data.waitSemaphores.PushBack(semaphore);
    data.waitStages.PushBack(stages);
    if (waitValue == 0)
        waitValue = semaphore->GetTempValue();
    //printf(" wait value %llu - Sema: %p\n", waitValue, semaphore->GetApiObject());
    data.timelineSemaWait.PushBack(waitValue);
}

void Renderer::RenderDevice::FlushQueues()
{
    this->FlushQueue(CommandBuffer::Type::ASYNC_TRANSFER);
    this->FlushQueue(CommandBuffer::Type::ASYNC_COMPUTE);
    this->FlushQueue(CommandBuffer::Type::GENERIC);
}

void Renderer::RenderDevice::ClearFrame()
{
    PerFrame& frame = Frame();
    const Internal::QueueFamilyIndices& queueFamilyIndices = this->GetQueueFamilyIndices();

    for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
        if (queueFamilyIndices.queueFamilies[i] != UINT32_MAX) {
            frame.commandPools[0][i].Reset();
            frame.submissions[i].Clear();
        }
    }

    this->DestroyPendingObjects(frame);
}

void Renderer::RenderDevice::BeginFrame()
{
    //printf("Begin Frame\n");

    //printf("Attempt to RESET: %d ", renderContext->GetCurrentFrame());
    stagingManager.ResetStage(renderContext->GetCurrentFrame());
    stagingManager.Flush();

    framebufferAllocator.BeginFrame();
    transientAttachmentAllocator.BeginFrame();

    for (auto& allocator : descriptorSetAllocators)
        allocator.second.BeginFrame();

    this->ClearFrame();
    submitSwapchain = false;
}

void Renderer::RenderDevice::EndFrame()
{
    this->FlushQueue(CommandBuffer::Type::ASYNC_TRANSFER);
    this->FlushQueue(CommandBuffer::Type::ASYNC_COMPUTE);
    // if we already did sumbit to swapchain then done force the swap
    this->FlushQueue(CommandBuffer::Type::GENERIC, !submitSwapchain);

    //printf("End Frame\n");
    //getchar();
}

Renderer::BlasHandle Renderer::RenderDevice::CreateBlas(const BlasCreateInfo& blasInfo, VkBuildAccelerationStructureFlagsKHR flags)
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
    vkGetAccelerationStructureBuildSizesKHR(this->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
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

Renderer::TlasHandle Renderer::RenderDevice::CreateTlas(const TlasCreateInfo& createInfo, VkBuildAccelerationStructureFlagsKHR flags)
{
    BufferInfo bufferInfo;
    bufferInfo.size = createInfo.blasInstances.size() * sizeof(VkAccelerationStructureInstanceKHR);
    bufferInfo.usage = BufferUsage::SHADER_DEVICE_ADDRESS;
    bufferInfo.domain = MemoryUsage::GPU_ONLY;
    BufferHandle instanceBuffer = this->CreateBuffer(bufferInfo);
    VkDeviceAddress instanceAddress = this->GetBufferAddress(instanceBuffer);

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
        this->GetDevice(),
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

VkAccelerationStructureKHR Renderer::RenderDevice::CreateAcceleration(VkAccelerationStructureCreateInfoKHR& info, BufferHandle* buffer)
{
    BufferCreateInfo bufferInfo;
    bufferInfo.size = info.size;
    bufferInfo.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_STORAGE;
    bufferInfo.domain = MemoryUsage::GPU_ONLY;
    *buffer = this->CreateBuffer(bufferInfo);
    info.buffer = (*buffer)->GetApiObject();

    VkAccelerationStructureKHR accl;
    CALL_VK(vkCreateAccelerationStructureKHR(this->GetDevice(), &info, NULL, &accl));
    return accl;
}

Renderer::CommandPoolHandle Renderer::RenderDevice::RequestCommandPool(uint32 queueFamily, Renderer::CommandPool::Type type)
{
    uint32 familyIndex;
    CommandBuffer::Type cmdType;

    switch (queueFamily){
    case Internal::QFT_TRANSFER:
        cmdType = CommandBuffer::Type::ASYNC_TRANSFER;
        break;
    case Internal::QFT_COMPUTE:
        cmdType = CommandBuffer::Type::ASYNC_TRANSFER;
        break;
    default:
        cmdType = CommandBuffer::Type::GENERIC;
    }

    for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
        if (Internal::QUEUE_FAMILY_FLAGS[i] & queueFamily) {
            familyIndex = this->GetQueueFamilyIndices().queueFamilies[i];
            break;
        }
    }

    auto handle = CommandPoolHandle(objectsPool.commandPools.Allocate(this, familyIndex, cmdType, (uint32)type));
    return handle;
}

Renderer::FenceHandle Renderer::RenderDevice::RequestFence()
{
    VkFence fence = fenceManager.RequestClearedFence();
    FenceHandle h(objectsPool.fences.Allocate(*this, fence));
    return h;
}

void Renderer::RenderDevice::ResetFence(VkFence fence, bool isWaited)
{
    if (isWaited) {
        vkResetFences(this->GetDevice(), 1, &fence);
        fenceManager.Recycle(fence);
    } else {
        Frame().recycleFences.EmplaceBack(fence);
        Frame().shouldDestroy = true;
    }
}

Renderer::SemaphoreHandle Renderer::RenderDevice::RequestSemaphore()
{
    VkSemaphore sem = semaphoreManager.RequestSemaphore();
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, sem));
    return ptr;
}

Renderer::SemaphoreHandle Renderer::RenderDevice::RequestTimelineSemaphore(uint64 value)
{
    VkSemaphore sem = semaphoreManager.RequestTimelineSemaphore(value);
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, sem, value));
    return ptr;
}

void Renderer::RenderDevice::ResetTimelineSemaphore(Renderer::Semaphore& semaphore)
{
    this->DestroySemaphore(semaphore.GetApiObject());
    VkSemaphore sem = semaphoreManager.RequestTimelineSemaphore(semaphore.initialValue);
    semaphore.semaphore = sem;
    semaphore.tempValue = semaphore.initialValue;
}

Renderer::PiplineEventHandle Renderer::RenderDevice::RequestPiplineEvent()
{
    VkEvent event = eventManager.RequestEvent();
    PiplineEventHandle ptr(objectsPool.events.Allocate(*this, event));
    return ptr;
}

Renderer::ImageHandle Renderer::RenderDevice::CreateImage(const ImageCreateInfo& createInfo, const void* data)
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
            queueFamilyIndices.AllocateInit(1, this->GetQueueFamilyIndices().queueFamilies[i]);
        }
    }

    if (info.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        info.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
        info.pQueueFamilyIndices = queueFamilyIndices.GetData();
    }

    VkImage apiImage;
    if (vkCreateImage(this->GetDevice(), &info, NULL, &apiImage) != VK_SUCCESS) {
        ASSERTF(true, "failed to create a image!");
    }

    MemoryView imageMemory;
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(this->GetDevice(), apiImage, &memRequirements);
    uint32 memoryTypeIndex = this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, memUsage);
    imageMemory = gpuMemoryAllocator.Allocate(memoryTypeIndex, memRequirements.size, memRequirements.alignment);
    vkBindImageMemory(this->GetDevice(), apiImage, imageMemory.memory, imageMemory.offset);

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

Renderer::ImageViewHandle Renderer::RenderDevice::CreateImageView(const ImageViewCreateInfo& createInfo)
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
    vkCreateImageView(this->GetDevice(), &viewInfo, NULL, &apiImageView);

    ImageViewHandle ret(objectsPool.imageViews.Allocate(*this, apiImageView, info));
    return ret;
}

bool Renderer::RenderDevice::CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo)
{
    outBuffer = this->CreateBufferHelper(createInfo);

    // TODO: fix this!! this is quick and dirty way
    if (!(createInfo.usage & BufferUsage::SHADER_DEVICE_ADDRESS)) {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->GetDevice(), outBuffer, &memRequirements);
        uint32 memoryTypeIndex = this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, createInfo.domain);
        outMemoryView = gpuMemoryAllocator.Allocate(memoryTypeIndex, createInfo.size, memRequirements.alignment);
        vkBindBufferMemory(this->GetDevice(), outBuffer, outMemoryView.memory, outMemoryView.offset);
    } else {
        outMemoryView.offset = 0;
        outMemoryView.size = createInfo.size;
        outMemoryView.padding = 0;
        outMemoryView.mappedData = NULL;
        outMemoryView.alignment = 0;

        outMemoryView.memory = this->CreateBufferMemory(createInfo, outBuffer);

        if (createInfo.domain == MemoryUsage::CPU_ONLY || createInfo.domain == MemoryUsage::CPU_CACHED || createInfo.domain == MemoryUsage::CPU_COHERENT) {
            vkMapMemory(this->GetDevice(), outMemoryView.memory, 0, createInfo.size, 0, &outMemoryView.mappedData);
        }
    }

    return true;
}

Renderer::BufferHandle Renderer::RenderDevice::CreateBuffer(const BufferInfo& createInfo, const void* data)
{
    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    this->CreateBufferInternal(apiBuffer, bufferMemory, createInfo);
    BufferHandle ret(objectsPool.buffers.Allocate(*this, apiBuffer, createInfo, bufferMemory));

    /*if (data) {
        if (createInfo.domain == MemoryUsage::CPU_ONLY || createInfo.domain == MemoryUsage::CPU_CACHED || createInfo.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }*/

    if (data)
        ret->WriteToBuffer(createInfo.size, data);

    return ret;
}

Renderer::BufferHandle Renderer::RenderDevice::CreateRingBuffer(const BufferInfo& createInfo, const uint32 ringSize, const void* data)
{
    BufferInfo info = createInfo;
    const DeviceSize alignment = this->internal.gpuProperties.limits.minUniformBufferOffsetAlignment;
    const DeviceSize padding = (alignment - (info.size % alignment)) % alignment;
    const DeviceSize alignedSize = info.size + padding;
    info.size = alignedSize * ringSize; //- padding; // here we must remove padding as we dont need it at the end but (otherwise waste of memory)
                                                     // this is going to complicate our calulations later so better keep it
    MemoryView bufferMemory;
    VkBuffer apiBuffer;

    // Removing padding from total size, as we dont need the last bytes for alignement
    // alignedSize * NUM_FRAMES - padding, data, usage, memoryUsage, queueFamilies
    this->CreateBufferInternal(apiBuffer, bufferMemory, info);
    BufferHandle ret(objectsPool.buffers.Allocate(*this, apiBuffer, info, bufferMemory, (uint32)alignedSize, ringSize));

    /*if (data) {
        if (info.domain == MemoryUsage::CPU_ONLY || info.domain == MemoryUsage::CPU_CACHED || info.domain == MemoryUsage::CPU_COHERENT) {
            ret->WriteToBuffer(createInfo.size, data);
        } else {
            stagingManager.Stage(ret->apiBuffer, data, createInfo.size, bufferMemory.alignment);
        }
    }*/

    if (data)
        ret->WriteToBuffer(createInfo.size, data);

    return ret;
}

Renderer::SamplerHandle Renderer::RenderDevice::CreateSampler(const SamplerInfo& createInfo)
{
    VkSampler sampler;
    VkSamplerCreateInfo info;
    SamplerInfo::FillVkSamplerCreateInfo(createInfo, info);
    vkCreateSampler(this->GetDevice(), &info, NULL, &sampler);
    SamplerHandle ret(objectsPool.samplers.Allocate(*this, sampler, createInfo));
    return ret;
}

Renderer::CommandBufferHandle Renderer::RenderDevice::RequestCommandBuffer(CommandBuffer::Type type)
{
    PerFrame& frame = Frame();
    auto handle = frame.commandPools[0][(uint32)type].RequestCommandBuffer();
    return handle;
}

Renderer::SemaphoreHandle Renderer::RenderDevice::GetImageAcquiredSemaphore()
{
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, renderContext->GetImageAcquiredSemaphore()));
    ptr->SetNoClean();
    return ptr;
}

Renderer::SemaphoreHandle Renderer::RenderDevice::GetDrawCompletedSemaphore()
{
    SemaphoreHandle ptr(objectsPool.semaphores.Allocate(*this, renderContext->GetDrawCompletedSemaphore()));
    ptr->SetNoClean();
    return ptr;
}

Renderer::DescriptorSetAllocator* Renderer::RenderDevice::RequestDescriptorSetAllocator(const DescriptorSetLayout& layout)
{
    Hasher h;
    h.Data(reinterpret_cast<const uint32*>(layout.GetDescriptorSetLayoutBindings()), sizeof(VkDescriptorSetLayoutBinding) * layout.GetBindingsCount());

    // For the weird return value check: https://en.cppreference.com/w/cpp/container/unordered_map/emplace
    const auto ctor_arg = std::pair<RenderDevice*, const DescriptorSetLayout&>(this, layout);
    const auto& res = descriptorSetAllocators.emplace(h.Get(), ctor_arg);

    if (res.second) {
        res.first->second.Init();
    }

    return &res.first->second;
}

Renderer::Pipeline& Renderer::RenderDevice::RequestPipeline(ShaderProgram& program, const RenderPass& rp, const GraphicsState& state)
{
   return pipelineAllocator.RequestPipline(program, rp, state);
}

Renderer::Pipeline& Renderer::RenderDevice::RequestPipeline(ShaderProgram& program)
{
    return pipelineAllocator.RequestPipline(program);
}

void Renderer::RenderDevice::CreateShaderProgram(const std::initializer_list<ShaderProgram::ShaderStage>& shaderStages, ShaderProgram* shaderProgramOut)
{
    // shaderProgramOut->Create(*this, shaderStages);
}

const Renderer::RenderPass& Renderer::RenderDevice::RequestRenderPass(const RenderPassInfo& info, bool compatible)
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

    auto rp2 = renderPasses.emplace(hash, RenderPass(*this, info));
    // printf("Creating render pass ID: %llu.\n", hash);
    rp2.first->second.hash = h.Get();
    return rp2.first->second;
}

const Renderer::Framebuffer& Renderer::RenderDevice::RequestFramebuffer(const RenderPassInfo& info, const RenderPass* rp)
{
    if (!rp) {
        rp = &this->RequestRenderPass(info);
    }

    return framebufferAllocator.RequestFramebuffer(*rp, info);
}

Renderer::RenderPassInfo Renderer::RenderDevice::GetSwapchainRenderPass(SwapchainRenderPass style)
{
    const auto& swapchain = renderContext->GetSwapchain();
    uint32 msaaSamplerCount = 1; // TODO: Change this!

    RenderPassInfo info;
    info.colorAttachmentCount = 1;
    info.colorAttachments[0] = swapchain.GetSwapchainImage(renderContext->GetCurrentImageIndex())->GetView();
    info.clearColor[0] = { 0.051f, 0.051f, 0.051f, 0.0f };
    info.clearAttachments = 1u << 0;
    info.storeAttachments = 1u << 0;

    switch (style) {
    case SwapchainRenderPass::DEPTH:
    {
        info.opFlags |= RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT;
        info.depthStencil =
            &GetTransientAttachment(swapchain.GetExtent().width,
                swapchain.GetExtent().height, swapchain.FindSupportedDepthFormat(), 0, msaaSamplerCount);
        break;
    }

    case SwapchainRenderPass::DEPTH_STENCIL:
    {
        info.opFlags |= RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT;
        info.depthStencil =
            &GetTransientAttachment(swapchain.GetExtent().width,
                swapchain.GetExtent().height, swapchain.FindSupportedDepthStencilFormat(), 0, msaaSamplerCount);
        break;
    }
    default:
        break;
    }

    return info;
}

Renderer::ImageView& Renderer::RenderDevice::GetTransientAttachment(uint32 width, uint32 height, VkFormat format, uint32 index, uint32 samples, uint32 layers)
{
    return transientAttachmentAllocator.RequestAttachment(width, height, format, index, samples, layers);
}

void Renderer::RenderDevice::DestroyPendingObjects(PerFrame& frame)
{
    if (!frame.shouldDestroy)
        return;

    VkDevice dev = this->GetDevice();

    for (const auto& kv : frame.destroyedCmdBuffers)
        vkFreeCommandBuffers(dev, kv.first, kv.second.size(), kv.second.data());

    for (auto pool : frame.destroyedCmdPools)
        vkDestroyCommandPool(dev, pool, NULL);

    for (auto rp : frame.destroyedRenderPasses)
        vkDestroyRenderPass(dev, rp, NULL);

    for (auto dsc : frame.destroyedDescriptorPool)
        vkDestroyDescriptorPool(dev, dsc, NULL);

    for (auto fb : frame.destroyedFramebuffers)
        vkDestroyFramebuffer(dev, fb, NULL);

    for (auto view : frame.destroyedImageViews)
        vkDestroyImageView(dev, view, NULL);

    for (auto img : frame.destroyedImages)
        vkDestroyImage(dev, img, NULL);

    for (auto view : frame.destroyedBufferViews)
        vkDestroyBufferView(dev, view, NULL);

    for (auto buff : frame.destroyedBuffers)
        vkDestroyBuffer(dev, buff, NULL);

    for (auto sem : frame.destroyedSemaphores)
        vkDestroySemaphore(dev, sem, NULL);

    for (auto sampler : frame.destroyedSamplers)
        vkDestroySampler(dev, sampler, NULL);

    if (enabledFeatures & RAY_TRACING) {
        for (auto accl : frame.destroyedAccls)
            vkDestroyAccelerationStructureKHR(dev, accl, NULL);

        frame.destroyedAccls.Clear();
    }

    // Free memory:
    for (auto& mem : frame.freedMemory)
        vkFreeMemory(dev, mem, NULL);

    // Free allocated memory:
    for (auto allocKey : frame.freeAllocatedMemory)
        gpuMemoryAllocator.Free(allocKey);

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
    frame.destroyedCmdPools.Clear();
    frame.destroyedCmdBuffers.clear();
    frame.shouldDestroy = false;
}

void Renderer::RenderDevice::DestroyImage(VkImage image)
{
    PerFrame& frame = this->Frame();
    frame.destroyedImages.EmplaceBack(image);
    frame.shouldDestroy = true;
}

void Renderer::RenderDevice::DestroyImageView(VkImageView view)
{
    PerFrame& frame = this->Frame();
    frame.destroyedImageViews.EmplaceBack(view);
    frame.shouldDestroy = true;
}

void Renderer::RenderDevice::DestroyFramebuffer(VkFramebuffer fb)
{
    PerFrame& frame = this->Frame();
    frame.destroyedFramebuffers.EmplaceBack(fb);
    frame.shouldDestroy = true;
}

void Renderer::RenderDevice::FreeMemory(VkDeviceMemory memory)
{
    PerFrame& frame = this->Frame();
    frame.freedMemory.EmplaceBack(memory);
    frame.shouldDestroy = true;
}

void Renderer::RenderDevice::FreeMemory(Renderer::MemoryAllocator::AllocKey key)
{
    PerFrame& frame = this->Frame();
    frame.freeAllocatedMemory.EmplaceBack(key);
    frame.shouldDestroy = true;
}

void Renderer::RenderDevice::RecycleSemaphore(VkSemaphore sem)
{
    Frame().recycleSemaphores.EmplaceBack(sem);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroySemaphore(VkSemaphore sem)
{
    Frame().destroyedSemaphores.EmplaceBack(sem);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroryEvent(VkEvent event)
{
    Frame().destroyedEvents.EmplaceBack(event);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroyBuffer(VkBuffer buffer)
{
    Frame().destroyedBuffers.EmplaceBack(buffer);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroyBufferView(VkBufferView view)
{
    Frame().destroyedBufferViews.EmplaceBack(view);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroySampler(VkSampler sampler)
{
    Frame().destroyedSamplers.EmplaceBack(sampler);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::FreeCommandBuffer(VkCommandPool pool, VkCommandBuffer cmd)
{
    Frame().destroyedCmdBuffers[pool].emplace_back(cmd);
    Frame().shouldDestroy = true;
}

void Renderer::RenderDevice::DestroyCommandPool(VkCommandPool pool)
{
    Frame().destroyedCmdPools.EmplaceBack(pool);
    Frame().shouldDestroy = true;
}


void Renderer::RenderDevice::DestroyAllFrames()
{
    objectsPool.commandPools.Clear();
    objectsPool.commandBuffers.Clear();
    objectsPool.buffers.Clear();
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

    for (PerFrame& frame : perFrame) {
        frame.shouldDestroy = true;

        for (uint32 i = 0; i < (uint32)CommandBuffer::Type::MAX; i++) {
            for (uint32 t = 0; t < MAX_THREADS; t++)
                frame.commandPools[t][i].Destroy();

            for (auto& sub : frame.submissions[i])
                sub.Clear();
        }
    }

    for (PerFrame& frame : perFrame) {
        this->DestroyPendingObjects(frame);
    }
}


TRE_NS_END
