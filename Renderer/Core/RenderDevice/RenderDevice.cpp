#include "RenderDevice.hpp"
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <unordered_set>
#include <Renderer/Core/Common/Utils.hpp>

TRE_NS_START

int32 Renderer::CreateRenderDevice(RenderDevice& renderDevice, const RenderInstance& renderInstance, const RenderContext& ctx)
{
    renderDevice.gpu = PickGPU(renderInstance, ctx);
    ASSERTF(renderDevice.gpu == VK_NULL_HANDLE, "Couldn't pick a GPU.");

    renderDevice.queueFamilyIndices     = FindQueueFamilies(renderDevice.gpu, ctx.surface);
    renderDevice.isPresentQueueSeprate  = renderDevice.queueFamilyIndices.queueFamilies[QFT_GRAPHICS] != 
                                            renderDevice.queueFamilyIndices.queueFamilies[QFT_PRESENT];

    vkGetPhysicalDeviceMemoryProperties(renderDevice.gpu, &renderDevice.memoryProperties);

    return CreateLogicalDevice(renderDevice, renderInstance, ctx);
}

void Renderer::DestroryRenderDevice(RenderDevice& renderDevice)
{
    vkDestroyDevice(renderDevice.device, NULL);
}

int32 Renderer::CreateLogicalDevice(RenderDevice& renderDevice, const RenderInstance& renderInstance, const RenderContext& ctx)
{
    ASSERT(renderInstance.instance == VK_NULL_HANDLE);

    VkDevice device = VK_NULL_HANDLE;
    float queuePriority = 1.0f;
    const QueueFamilyIndices& indices = renderDevice.queueFamilyIndices;

    TRE::Vector<VkDeviceQueueCreateInfo> queueCreateInfos(QFT_MAX);
    std::unordered_set<uint32> uniqueQueueFamilies(std::begin(indices.queueFamilies), std::end(indices.queueFamilies));

    for (uint32 queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.EmplaceBack(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos        = queueCreateInfos.Data();
    createInfo.queueCreateInfoCount     = (uint32)queueCreateInfos.Size();

    createInfo.pEnabledFeatures         = &deviceFeatures;

    createInfo.enabledExtensionCount    = (uint32)VK_REQ_DEVICE_EXT.size();
    createInfo.ppEnabledExtensionNames  = VK_REQ_DEVICE_EXT.begin();

    createInfo.enabledLayerCount        = (uint32)VK_REQ_LAYERS.size();
    createInfo.ppEnabledLayerNames      = VK_REQ_LAYERS.begin();

    VkResult res = vkCreateDevice(renderDevice.gpu, &createInfo, NULL, &renderDevice.device);

    if (res != VK_SUCCESS) {
        ASSERTF(true, "Couldn't create a logical device (%s)!", GetVulkanResultString(res));
        return -1;
    }

    for (uint32 queueIndex = 0; queueIndex < QFT_MAX; queueIndex++) {
        if (indices.queueFamilies[queueIndex] != uint32(-1)) {
            vkGetDeviceQueue(renderDevice.device, indices.queueFamilies[queueIndex], 0, &renderDevice.queues[queueIndex]);
        }
    }

    return 0;
}

VkPhysicalDevice Renderer::PickGPU(const RenderInstance& renderInstance, const RenderContext& ctx, FPN_RankGPU p_pick_func)
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

bool Renderer::IsDeviceSuitable(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(gpu, surface);
    
    bool swapChainAdequate = false;
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu, surface);
    swapChainAdequate = !swapChainSupport.formats.IsEmpty() && !swapChainSupport.presentModes.IsEmpty();

	return indices.IsComplete() && swapChainAdequate;
}

Renderer::QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice p_gpu, VkSurfaceKHR p_surface)
{
    QueueFamilyIndices indices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p_gpu, &queueFamilyCount, NULL);

    TRE::Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount, {});
    vkGetPhysicalDeviceQueueFamilyProperties(p_gpu, &queueFamilyCount, queueFamilies.Data());

    int32 i = 0;

    for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.queueFamilies[QFT_GRAPHICS] = i;
        }

        VkBool32 presentSupport = false;

        if (p_surface) {
            vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu, i, p_surface, &presentSupport);

            if (presentSupport) {
                indices.queueFamilies[QFT_PRESENT] = i;
            }
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

TRE_NS_END