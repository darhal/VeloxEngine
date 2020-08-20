#include "SwapChain.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>

TRE_NS_START

Renderer::Internal::SwapChainSupportDetails Renderer::Internal::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
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

void Renderer::Internal::CreateSwapChain(const RenderDevice& renderDevice, RenderContext& ctx)
{
    ASSERT(renderDevice.gpu == VK_NULL_HANDLE);
    ASSERT(ctx.surface == VK_NULL_HANDLE);

    uint32 width = ctx.window->getSize().x;
    uint32 height = ctx.window->getSize().y;
    VkSwapchainKHR oldSwapChain = ctx.swapChain;

    ctx.swapChainData.swapChainExtent        = VkExtent2D{ width, height };
    
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(renderDevice.gpu, ctx.surface);
    VkSurfaceFormatKHR surfaceFormat         = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode             = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent                        = ChooseSwapExtent(swapChainSupport.capabilities, ctx.swapChainData.swapChainExtent);
    ctx.imagesCount                          = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && ctx.imagesCount > swapChainSupport.capabilities.maxImageCount) {
        ctx.imagesCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR    createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = ctx.surface;
    createInfo.minImageCount    = ctx.imagesCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    const QueueFamilyIndices& indices = renderDevice.queueFamilyIndices;
    uint32 queueFamilyIndices[]       = { indices.queueFamilies[QFT_GRAPHICS], indices.queueFamilies[QFT_PRESENT] };

    if (indices.queueFamilies[QFT_GRAPHICS] != indices.queueFamilies[QFT_PRESENT]) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode      = presentMode;
    createInfo.clipped          = VK_TRUE;
    createInfo.oldSwapchain     = oldSwapChain;

    if (vkCreateSwapchainKHR(renderDevice.device, &createInfo, NULL, &ctx.swapChain) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &ctx.imagesCount, NULL);
    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &ctx.imagesCount, ctx.swapChainData.swapChainImages);
    
    ctx.swapChainData.swapChainImageFormat = surfaceFormat.format;
    ctx.swapChainData.swapChainExtent      = extent;

    if (oldSwapChain == VK_NULL_HANDLE) {
        CreateSyncObjects(renderDevice, ctx);
        CreateSwapChainRenderPass(renderDevice, ctx);
        CreateCommandPool(renderDevice, ctx);
        CreateCommandBuffers(renderDevice, ctx);
    } else {
        vkDestroySwapchainKHR(renderDevice.device, oldSwapChain, NULL);
    }

    CreateSwapChainResources(renderDevice, ctx);
}

void Renderer::Internal::DestroySwapChain(const RenderDevice& renderDevice, RenderContext& ctx)
{
    CleanupSwapChain(ctx, renderDevice);

    vkDestroySwapchainKHR(renderDevice.device, ctx.swapChain, NULL);
    ctx.swapChain = VK_NULL_HANDLE;

    for (size_t i = 0; i < SwapChainData::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(renderDevice.device, ctx.swapChainData.drawCompleteSemaphores[i], NULL);
        vkDestroySemaphore(renderDevice.device, ctx.swapChainData.imageAcquiredSemaphores[i], NULL);
        vkDestroyFence(renderDevice.device, ctx.swapChainData.fences[i], NULL);
    }

    vkDestroyCommandPool(renderDevice.device, ctx.contextData.commandPool, NULL);
}

void Renderer::Internal::CleanupSwapChain(RenderContext& ctx, const RenderDevice& renderDevice)
{
    SwapChainData& swapChainData   = ctx.swapChainData;
    VkDevice device                 = renderDevice.device;

    for (size_t i = 0; i < ctx.imagesCount; i++) {
        vkDestroyFramebuffer(device, swapChainData.swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device, swapChainData.swapChainImageViews[i], NULL);
    }

    // vkFreeCommandBuffers(device, swapChainData.commandPool, static_cast<uint32_t>(swapChainData.commandBuffers.Size()), swapChainData.commandBuffers.Data());

    // vkDestroyPipeline(device, swapChainData.graphicsPipeline, NULL);
    // vkDestroyPipelineLayout(device, swapChainData.pipelineLayout, NULL);
    // vkDestroyRenderPass(device, swapChainData.renderPass, NULL);
}

void Renderer::Internal::RecreateSwapChainInternal(const RenderDevice& renderDevice, RenderContext& ctx)
{
    vkDeviceWaitIdle(renderDevice.device);
    CleanupSwapChain(ctx, renderDevice);

    uint32 width = ctx.window->getSize().x;
    uint32 height = ctx.window->getSize().y;

    while (width == 0 || height == 0) {
        ctx.window->WaitEvents();

        width = ctx.window->getSize().x;
        height = ctx.window->getSize().y;
    }

    // ctx.framebufferResized = false;
    CreateSwapChain(renderDevice, ctx);
}

void Renderer::Internal::UpdateSwapChain(RenderContext& ctx)
{
    ctx.framebufferResized = true;
    // RecreateSwapChainInternal(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
}


void Renderer::Internal::CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx)
{
    ctx.currentFrame = 0;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < SwapChainData::MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.imageAcquiredSemaphores[i]);
        vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.drawCompleteSemaphores[i]);
        vkCreateFence(renderDevice.device, &fenceInfo, NULL, &ctx.swapChainData.fences[i]);

        if (renderDevice.isPresentQueueSeprate) {
            vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.imageOwnershipSemaphores[i]);
        }

        if (renderDevice.isTransferQueueSeprate) {
            vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.transferSemaphores[i]);
        }
    }

    fenceInfo.flags = 0;
    vkCreateFence(renderDevice.device, &fenceInfo, NULL, &ctx.swapChainData.transferSyncFence);
}

void Renderer::Internal::CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx)
{
    const QueueFamilyIndices& queueFamilyIndices = renderDevice.queueFamilyIndices;

    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_GRAPHICS];
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.commandPool) != VK_SUCCESS) {
            ASSERTF(true, "failed to create command pool!");
        }
    }

    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_TRANSFER];
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.memoryCommandPool) != VK_SUCCESS) {
            ASSERTF(true, "failed to create command pool!");
        }
    }

    if (renderDevice.isPresentQueueSeprate) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_PRESENT];
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.presentCommandPool) != VK_SUCCESS) {
            ASSERTF(true, "failed to create command pool!");
        }
    }
}

void Renderer::Internal::CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx)
{
    SwapChainData& swapChainData = ctx.swapChainData;
    RenderContextData& ctxData = ctx.contextData;

    {
        VkCommandBufferAllocateInfo allocInfo[3] = { {} };
        allocInfo[0].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo[0].commandPool           = ctxData.commandPool;
        allocInfo[0].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo[0].commandBufferCount    = 1;

        allocInfo[1].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo[1].commandPool           = ctxData.memoryCommandPool;
        allocInfo[1].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo[1].commandBufferCount    = 1;

        allocInfo[2].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo[2].commandPool           = ctxData.presentCommandPool;
        allocInfo[2].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo[2].commandBufferCount    = 1;

        for (uint32 i = 0; i < SwapChainData::MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[0], &ctxData.contextFrameResources[i].graphicsCommandBuffer) != VK_SUCCESS) {
                ASSERTF(true, "failed to allocate command buffers!");
            }

            if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[1], &ctxData.contextFrameResources[i].transferCommandBuffer) != VK_SUCCESS) {
                ASSERTF(true, "failed to allocate command buffers!");
            }

            if (renderDevice.isPresentQueueSeprate) {
                if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[2], &ctxData.contextFrameResources[i].presentCommandBuffer) != VK_SUCCESS) {
                    ASSERTF(true, "failed to allocate command buffers!");
                }

                BuildImageOwnershipCmd(renderDevice, ctx, i);
            }
        }
    }
}

void Renderer::Internal::CreateSwapChainResources(const RenderDevice& renderDevice, RenderContext& ctx)
{
    SwapChainData& swapChainData = ctx.swapChainData;

    for (size_t i = 0; i < ctx.imagesCount; i++) {
        // Create Image view:
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainData.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainData.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderDevice.device, &createInfo, NULL, &swapChainData.swapChainImageViews[i]) != VK_SUCCESS) {
            ASSERTF(true, "Failed to create image views!");
        }

        // Create framebuffers:
        VkImageView attachments[] = {
            swapChainData.swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = ctx.contextData.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = ctx.swapChainData.swapChainExtent.width;
        framebufferInfo.height = ctx.swapChainData.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(renderDevice.device, &framebufferInfo, NULL, &swapChainData.swapChainFramebuffers[i]) != VK_SUCCESS) {
            ASSERTF(true, "failed to create framebuffer!");
        }
    }
}

void Renderer::Internal::CreateSwapChainRenderPass(const RenderDevice& renderDevice, RenderContext& ctx)
{
    RenderPassDesc renderpassDesc;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format          = ctx.swapChainData.swapChainImageFormat;
    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkSubpassDependency dependency{};
    dependency.srcSubpass       = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass       = 0;
    dependency.srcStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask    = 0;
    dependency.dstStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment   = 0;
    colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;

    renderpassDesc.attachments.EmplaceBack(colorAttachment);
    renderpassDesc.subpassDependency.EmplaceBack(dependency);
    renderpassDesc.subpassesDesc.EmplaceBack(subpass);

    CreateRenderPass(renderDevice, &ctx.contextData.renderPass, renderpassDesc);
}

void Renderer::Internal::BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex)
{
    VkCommandBuffer presentCommandBuffer = ctx.contextData.contextFrameResources[imageIndex].presentCommandBuffer;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(presentCommandBuffer, &beginInfo);

    VkImageMemoryBarrier imageOwnershipBarrier{};
    imageOwnershipBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageOwnershipBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageOwnershipBarrier.srcQueueFamilyIndex = renderDevice.queueFamilyIndices.queueFamilies[QFT_GRAPHICS];
    imageOwnershipBarrier.dstQueueFamilyIndex = renderDevice.queueFamilyIndices.queueFamilies[QFT_PRESENT];
    imageOwnershipBarrier.image = ctx.swapChainData.swapChainImages[imageIndex];
    imageOwnershipBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(presentCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VkDependencyFlagBits{}, 0, NULL, 0, NULL, 1, &imageOwnershipBarrier);

    vkEndCommandBuffer(presentCommandBuffer);
}

VkSurfaceFormatKHR Renderer::Internal::ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Renderer::Internal::ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {        
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::Internal::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent)
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
