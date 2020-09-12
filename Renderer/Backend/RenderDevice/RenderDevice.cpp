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

int32 Renderer::RenderDevice::CreateRenderDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx)
{
    internal.gpu = PickGPU(renderInstance, ctx);
    ASSERTF(internal.gpu == VK_NULL_HANDLE, "Couldn't pick a GPU.");

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

int32 Renderer::RenderDevice::CreateLogicalDevice(const Internal::RenderInstance& renderInstance, const Internal::RenderContext& ctx)
{
    ASSERT(renderInstance.instance == VK_NULL_HANDLE);

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

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE; // TODO: check if the GPU supports this feature

    VkDeviceCreateInfo createInfo{};
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos        = queueCreateInfos.Data();
    createInfo.queueCreateInfoCount     = (uint32)queueCreateInfos.Size();

    createInfo.pEnabledFeatures         = &deviceFeatures;

    createInfo.enabledExtensionCount    = (uint32)VK_REQ_DEVICE_EXT.size();
    createInfo.ppEnabledExtensionNames  = VK_REQ_DEVICE_EXT.begin();

    createInfo.enabledLayerCount        = (uint32)VK_REQ_LAYERS.size();
    createInfo.ppEnabledLayerNames      = VK_REQ_LAYERS.begin();

    VkResult res = vkCreateDevice(internal.gpu, &createInfo, NULL, &internal.device);

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

bool Renderer::RenderDevice::IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    Internal::QueueFamilyIndices indices = FindQueueFamilies(gpu, surface);
    
    bool swapChainAdequate = false;
    Renderer::Swapchain::SwapchainSupportDetails swapChainSupport = Renderer::Swapchain::QuerySwapchainSupport(gpu, surface);
    swapChainAdequate = !swapChainSupport.formats.IsEmpty() && !swapChainSupport.presentModes.IsEmpty();

	return indices.IsComplete() && swapChainAdequate;
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
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.queueFamilies[Internal::QFT_GRAPHICS] = i;
        }

        if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
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

    printf("Complete! (Graphics: %d | Transfer: %d)\n", indices.queueFamilies[Internal::QFT_GRAPHICS], indices.queueFamilies[Internal::QFT_TRANSFER]);
    return indices;
}

TRE_NS_END