#include "SwapChain.hpp"
#include <Renderer/Core/PhysicalDevice/PhysicalDevice.hpp>
#include <Engine/Core/Misc/Maths/Maths.hpp>

TRE_NS_START

Renderer::SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

    if (formatCount != 0) {
        details.formats.Resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.Data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);

    if (presentModeCount != 0) {
        details.presentModes.Resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.Data());
    }

    return details;
}

void Renderer::CreateSwapChain(SwapChain& swapChain, const RenderContext& ctx)
{
    ASSERT(ctx.gpu == VK_NULL_HANDLE);
    ASSERT(ctx.surface == VK_NULL_HANDLE);
 
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(ctx.gpu, ctx.surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, swapChain.swapChainExtent);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = ctx.surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(ctx.gpu);
    uint32_t queueFamilyIndices[] = { indices.queueFamilies[QFT_GRAPHICS], indices.queueFamilies[QFT_PRESENT] };

    if (indices.queueFamilies[QFT_GRAPHICS] != indices.queueFamilies[QFT_PRESENT]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(ctx.device, &createInfo, NULL, &swapChain.swapChain) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(ctx.device, swapChain.swapChain, &imageCount, NULL);
    swapChain.swapChainImages.Resize(imageCount);
    vkGetSwapchainImagesKHR(ctx.device, swapChain.swapChain, &imageCount, swapChain.swapChainImages.Data());

    swapChain.swapChainImageFormat = surfaceFormat.format;
    swapChain.swapChainExtent = extent;

    CreateImageViews(ctx.device, swapChain);
    CreateSyncObjects(ctx.device, swapChain);
}

void Renderer::DestroySwapChain(const VkDevice device, SwapChain& swapChain)
{
    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, swapChain.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, swapChain.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, swapChain.inFlightFences[i], nullptr);
    }

    for (auto imageView : swapChain.swapChainImageViews) {
        vkDestroyImageView(device, imageView, NULL);
    }

    vkDestroySwapchainKHR(device, swapChain.swapChain, NULL);
}

void Renderer::Present(RenderContext& ctx, const TRE::Vector<VkCommandBuffer>& cmdbuff)
{
    VkDevice device = ctx.device;
    SwapChain& swapChain = ctx.swapChain;
    uint32 currentFrame = swapChain.currentFrame;

    vkWaitForFences(device, 1, &swapChain.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain.swapChain, UINT64_MAX, swapChain.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (swapChain.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &swapChain.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    swapChain.imagesInFlight[imageIndex] = swapChain.inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { swapChain.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = (uint32)cmdbuff.Size();
    submitInfo.pCommandBuffers = cmdbuff.Data();

    VkSemaphore signalSemaphores[] = { swapChain.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &swapChain.inFlightFences[currentFrame]);

    if (vkQueueSubmit(ctx.queues[QFT_GRAPHICS], 1, &submitInfo, swapChain.inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }


    // Presenting:
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(ctx.queues[QFT_PRESENT], &presentInfo);

    swapChain.currentFrame = (currentFrame + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::CreateImageViews(VkDevice device, SwapChain& swapChain)
{
    swapChain.swapChainImageViews.Resize(swapChain.swapChainImages.Size());

    for (size_t i = 0; i < swapChain.swapChainImages.Size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChain.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChain.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, NULL, &swapChain.swapChainImageViews[i]) != VK_SUCCESS) {
            ASSERTF(true, "Failed to create image views!");
        }
    }
}

void Renderer::CreateSyncObjects(VkDevice device, SwapChain& swapChain)
{
    swapChain.currentFrame = 0;

    swapChain.imageAvailableSemaphores.Resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    swapChain.renderFinishedSemaphores.Resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    swapChain.inFlightFences.Resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    // swapChain.imagesInFlight.Resize(swapChain.swapChainImages.Size(), VK_NULL_HANDLE);
    swapChain.imagesInFlight = TRE::Vector<VkFence>(swapChain.swapChainImages.Size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &swapChain.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, NULL, &swapChain.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, NULL, &swapChain.inFlightFences[i]) != VK_SUCCESS) {
            
            ASSERTF(true, "failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = extent;

        actualExtent.width = TRE::Math::Max<uint32>(
            capabilities.minImageExtent.width, 
            TRE::Math::Min<uint32>(capabilities.maxImageExtent.width, actualExtent.width)
        );
        actualExtent.height = TRE::Math::Max<uint32>(
            capabilities.minImageExtent.height, 
            TRE::Math::Min<uint32>(capabilities.maxImageExtent.height, actualExtent.height)
        );

        return actualExtent;
    }
}

TRE_NS_END
