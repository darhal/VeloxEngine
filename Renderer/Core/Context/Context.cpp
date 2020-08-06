#include "Context.hpp"
#include <Renderer/Window/Window.hpp>
#include <Renderer/Core/PhysicalDevice/PhysicalDevice.hpp>
#include <Renderer/Core/WindowSurface/WindowSurface.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Instance/Instance.hpp>
#include <unordered_set>

TRE_NS_START

void Renderer::InitContext(RenderContext& p_ctx)
{
    ASSERT(p_ctx.instance == VK_NULL_HANDLE);

    CreateWindowSurface(p_ctx);
	p_ctx.gpu = PickGPU(p_ctx);
    p_ctx.queueFamilyIndices = FindQueueFamilies(p_ctx.gpu, p_ctx.surface);

    if (p_ctx.gpu == VK_NULL_HANDLE) {
        fprintf(stderr, "Couldn't pick a GPU!\n");
    }

    CreateDevice(p_ctx);

    p_ctx.swapChain.swapChainExtent = VkExtent2D{ p_ctx.window->getSize().x, p_ctx.window->getSize().y };
    CreateSwapChain(p_ctx.swapChain, p_ctx);
}

void Renderer::DestroyContext(RenderContext& p_ctx)
{
    DestroySwapChain(p_ctx.device, p_ctx.swapChain);
    vkDestroyDevice(p_ctx.device, NULL);
    DestroryWindowSurface(p_ctx.instance->instance, p_ctx.surface);

    if (p_ctx.instance) {
        DestroyRenderInstance(*p_ctx.instance);
    }
}

int32 Renderer::CreateDevice(RenderContext& p_ctx)
{
    ASSERT(p_ctx.instance == VK_NULL_HANDLE);

    VkDevice device = VK_NULL_HANDLE;
    float queuePriority = 1.0f;
    const QueueFamilyIndices& indices = p_ctx.queueFamilyIndices;

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
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;


    createInfo.pQueueCreateInfos = queueCreateInfos.Data();
    createInfo.queueCreateInfoCount = (uint32)queueCreateInfos.Size();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = (uint32)VK_REQ_DEVICE_EXT.size();
    createInfo.ppEnabledExtensionNames = VK_REQ_DEVICE_EXT.begin();

    createInfo.enabledLayerCount = (uint32)VK_REQ_LAYERS.size();
    createInfo.ppEnabledLayerNames = VK_REQ_LAYERS.begin();

    if (vkCreateDevice(p_ctx.gpu, &createInfo, NULL, &p_ctx.device) != VK_SUCCESS) {
        fprintf(stderr, "Couldn't create a logical device!\n");
        return -1;
    }

    for (uint32 queueIndex = 0; queueIndex < QFT_MAX; queueIndex++) {
        if (indices.queueFamilies[queueIndex] != uint32(-1)) {
            vkGetDeviceQueue(p_ctx.device, indices.queueFamilies[queueIndex], 0, &p_ctx.queues[queueIndex]);
        }
    }

    return 0;
}

TRE_NS_END