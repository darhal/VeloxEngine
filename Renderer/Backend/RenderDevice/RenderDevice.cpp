#include "RenderDevice.hpp"
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <unordered_set>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>

TRE_NS_START

Renderer::RenderDevice::RenderDevice() : internal{ 0 }
{

}

int32 Renderer::RenderDevice::CreateRenderDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, 
    const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    internal.gpu = PickGPU(renderInstance, ctx);
    ASSERTF(internal.gpu == VK_NULL_HANDLE, "Couldn't pick a GPU.");

    this->FetchDeviceAvailableExtensions();
    vkGetPhysicalDeviceFeatures(internal.gpu, &internal.gpuFeatures);
    vkGetPhysicalDeviceProperties(internal.gpu, &internal.gpuProperties);

    internal.queueFamilyIndices     = FindQueueFamilies(internal.gpu, ctx.surface);
    internal.isPresentQueueSeprate  = internal.queueFamilyIndices.queueFamilies[Internal::QFT_GRAPHICS] != 
                                            internal.queueFamilyIndices.queueFamilies[Internal::QFT_PRESENT];
    internal.isTransferQueueSeprate = internal.queueFamilyIndices.queueFamilies[Internal::QFT_GRAPHICS] !=
                                            internal.queueFamilyIndices.queueFamilies[Internal::QFT_TRANSFER];

    vkGetPhysicalDeviceMemoryProperties(internal.gpu, &internal.memoryProperties);
    return CreateLogicalDevice(renderInstance, ctx);
}

void Renderer::RenderDevice::DestroryRenderDevice()
{
    vkDestroyDevice(internal.device, NULL);
}

int32 Renderer::RenderDevice::CreateLogicalDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx,
    const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    ASSERT(renderInstance.instance == VK_NULL_HANDLE);

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
        deviceExtensions.emplace();
    }

    for (const auto& ext : VK_REQ_LAYERS) {
        layersArr.PushBack(ext);
    }

    for (uint32 i = 0; i < layerCount; i++) {
        layersArr.PushBack(layers[i]);
    }

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
    // TODO: 
    VkPhysicalDeviceFeatures2 deviceFeatures2;
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = NULL;
    
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
    VkPhysicalDeviceBufferDeviceAddressFeatures buffAdrFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };

    deviceFeatures2.pNext = &accelFeatures;
    accelFeatures.pNext = &rtPipelineFeatures;
    rtPipelineFeatures.pNext = &buffAdrFeatures;
    deviceFeatures2.features.samplerAnisotropy = VK_TRUE;
    deviceFeatures2.features.fillModeNonSolid = VK_TRUE;
    // buffAdrFeatures.bufferDeviceAddress = VK_TRUE;
    vkGetPhysicalDeviceFeatures2(internal.gpu, &deviceFeatures2);
    
    // Classical VkPhysicalDeviceFeatures deviceFeatures{};
    // TODO: check if the GPU supports this feature

    VkDeviceCreateInfo createInfo;
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &deviceFeatures2;
    createInfo.flags = 0;
    createInfo.pEnabledFeatures = NULL;//&deviceFeatures2.features;

    createInfo.pQueueCreateInfos        = queueCreateInfos.Data();
    createInfo.queueCreateInfoCount     = (uint32)queueCreateInfos.Size();

    createInfo.enabledExtensionCount    = (uint32)extensionsArr.Size();
    createInfo.ppEnabledExtensionNames  = extensionsArr.begin();

    createInfo.enabledLayerCount        = (uint32)layersArr.Size();
    createInfo.ppEnabledLayerNames      = layersArr.begin();

    VkResult res = vkCreateDevice(internal.gpu, &createInfo, NULL, &internal.device);
    load_VK_EXTENSION_SUBSET(renderInstance.instance, vkGetInstanceProcAddr, internal.device, vkGetDeviceProcAddr);

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

VkDeviceMemory Renderer::RenderDevice::AllocateDedicatedMemory(VkImage image, MemoryUsage memoryDomain) const
{
    VkDeviceMemory memory;
    VkMemoryAllocateInfo info;
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(internal.device, image, &memRequirements);

    info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext           = NULL;
    info.allocationSize  = memRequirements.size;
    info.memoryTypeIndex = Buffer::FindMemoryTypeIndex(*this, memRequirements.memoryTypeBits, memoryDomain);

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
    info.memoryTypeIndex = Buffer::FindMemoryTypeIndex(*this, memRequirements.memoryTypeBits, memoryDomain);

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

VkPhysicalDevice Renderer::RenderDevice::PickGPU(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx, FPN_RankGPU p_pick_func)
{
    ASSERT(ctx.surface == NULL);
    ASSERT(renderInstance.instance == NULL);

    VkInstance instance = renderInstance.instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32 deviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "Failed to find GPUs with Vulkan support!\n");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount, VK_NULL_HANDLE);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices) {
        if (p_pick_func(device, ctx.surface)) {
            physicalDevice = device;
            break;
        }
    }

    return physicalDevice;
}

void Renderer::RenderDevice::FetchDeviceAvailableExtensions()
{
    uint32 extensionsCount;
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

	return indices.IsComplete() && swapChainAdequate && supportedFeatures.samplerAnisotropy;
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
        }

        /*if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && 
            !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
        {
            indices.queueFamilies[Internal::QFT_GRAPHICS] = i;
        }*/

        if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
            !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
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

    // printf("Complete! (Graphics: %d | Transfer: %d)\n", indices.queueFamilies[Internal::QFT_GRAPHICS], indices.queueFamilies[Internal::QFT_TRANSFER]);
    return indices;
}

TRE_NS_END