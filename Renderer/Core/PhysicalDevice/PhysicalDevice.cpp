#include "PhysicalDevice.hpp"
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

VkPhysicalDevice Renderer::PickGPU(const RenderContext& p_ctx, FPN_RankGPU p_pick_func)
{
    ASSERT(p_ctx.surface == NULL);
    ASSERT(p_ctx.instance == NULL);
    ASSERT(p_ctx.instance->instance == NULL);

    VkInstance instance = p_ctx.instance->instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32 deviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "Failed to find GPUs with Vulkan support!\n");
    }

    TRE::Vector<VkPhysicalDevice> devices(deviceCount, VK_NULL_HANDLE);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.Data());

    for (const VkPhysicalDevice& device : devices) {
        if (p_pick_func(device, p_ctx.surface)) {
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