#include "Swapchain.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/Images/ImageHelper.hpp>

TRE_NS_START

Renderer::Swapchain::Swapchain(const RenderDevice& renderDevice, RenderContext& renderContext) :
    renderDevice(renderDevice),
    renderContext(renderContext)
{
}

Renderer::Swapchain::SwapchainSupportDetails Renderer::Swapchain::QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapchainSupportDetails supportDetails;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

    if (formatCount != 0) {
        supportDetails.formats.Resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, supportDetails.formats.Data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);

    if (presentModeCount != 0) {
        supportDetails.presentModes.Resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, supportDetails.presentModes.Data());
    }

    return supportDetails;
}

void Renderer::Swapchain::CreateSwapchain()
{
    ASSERT(renderDevice.GetDevice() == VK_NULL_HANDLE);
    ASSERT(renderContext.GetSurface() == VK_NULL_HANDLE);

    uint32 width = renderContext.GetWindow()->getSize().x;
    uint32 height = renderContext.GetWindow()->getSize().y;
    VkSwapchainKHR oldSwapchain = swapchain;

    swapchainData.swapChainExtent = VkExtent2D{ width, height };
    
    supportDetails                           = QuerySwapchainSupport(renderDevice.GetGPU(), renderContext.GetSurface());
    VkSurfaceFormatKHR surfaceFormat         = ChooseSwapSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode             = ChooseSwapPresentMode(supportDetails.presentModes);
    VkExtent2D extent                        = ChooseSwapExtent(supportDetails.capabilities, swapchainData.swapChainExtent);
    imagesCount                              = supportDetails.capabilities.minImageCount + 1;

    if (supportDetails.capabilities.maxImageCount > 0 && imagesCount > supportDetails.capabilities.maxImageCount) {
        imagesCount = supportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR    createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = renderContext.GetSurface();
    createInfo.minImageCount    = imagesCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    const Internal::QueueFamilyIndices& indices = renderDevice.GetQueueFamilyIndices();
    uint32 queueFamilyIndices[]       = { indices.queueFamilies[Internal::QFT_GRAPHICS], indices.queueFamilies[Internal::QFT_PRESENT] };

    if (indices.queueFamilies[Internal::QFT_GRAPHICS] != indices.queueFamilies[Internal::QFT_PRESENT]) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform     = supportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode      = presentMode;
    createInfo.clipped          = VK_TRUE;
    createInfo.oldSwapchain     = oldSwapchain;

    if (vkCreateSwapchainKHR(renderDevice.GetDevice(), &createInfo, NULL, &swapchain) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(renderDevice.GetDevice(), swapchain, &imagesCount, NULL);
    vkGetSwapchainImagesKHR(renderDevice.GetDevice(), swapchain, &imagesCount, swapchainData.swapChainImages);
    
    swapchainData.swapChainImageFormat = surfaceFormat.format;
    swapchainData.swapChainExtent      = extent;

    if (oldSwapchain == VK_NULL_HANDLE) {
        CreateSyncObjects();
        CreateSwapchainRenderPass();
        // CreateCommandPool(renderDevice, ctx);
        // CreateCommandBuffers(renderDevice, ctx);
    } else {
        vkDestroySwapchainKHR(renderDevice.GetDevice(), oldSwapchain, NULL);
    }

    CreateSwapchainResources();
}

void Renderer::Swapchain::DestroySwapchain()
{
    CleanupSwapchain();

    vkDestroySwapchainKHR(renderDevice.GetDevice(), swapchain, NULL);
    swapchain = VK_NULL_HANDLE;

    for (size_t i = 0; i < renderContext.GetNumFrames(); i++) {
        vkDestroySemaphore(renderDevice.GetDevice(), swapchainData.drawCompleteSemaphores[i], NULL);
        vkDestroySemaphore(renderDevice.GetDevice(), swapchainData.imageAcquiredSemaphores[i], NULL);
        vkDestroyFence(renderDevice.GetDevice(), swapchainData.fences[i], NULL);
    }

    // TODO: verify this later
    // vkDestroyCommandPool(renderDevice.GetDevice(), ctx.contextData.commandPool, NULL);
}

void Renderer::Swapchain::CleanupSwapchain()
{
    VkDevice device = renderDevice.GetDevice();

    for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyFramebuffer(device, swapchainData.swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device, swapchainData.swapChainImageViews[i], NULL);
    }

    vkDestroyImageView(device, swapchainData.depthStencilIamgeView, NULL);
    vkDestroyImage(device, swapchainData.depthStencilImage, NULL);
    this->renderDevice.FreeDedicatedMemory(swapchainData.depthStencilImageMemory);
}

void Renderer::Swapchain::RecreateSwapchain()
{
    vkDeviceWaitIdle(renderDevice.GetDevice());
    CleanupSwapchain();

    uint32 width = renderContext.GetWindow()->getSize().x;
    uint32 height = renderContext.GetWindow()->getSize().y;

    while (width == 0 || height == 0) {
        renderContext.GetWindow()->WaitEvents();

        width = renderContext.GetWindow()->getSize().x;
        height = renderContext.GetWindow()->getSize().y;
    }

    // framebufferResized = false;
    CreateSwapchain();
}

void Renderer::Swapchain::UpdateSwapchain()
{
    framebufferResized = true;
    // RecreateSwapchainInternal(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
}

void Renderer::Swapchain::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < renderContext.GetNumFrames(); i++) {
        vkCreateSemaphore(renderDevice.GetDevice(), &semaphoreInfo, NULL, &swapchainData.imageAcquiredSemaphores[i]);
        vkCreateSemaphore(renderDevice.GetDevice(), &semaphoreInfo, NULL, &swapchainData.drawCompleteSemaphores[i]);
        vkCreateFence(renderDevice.GetDevice(), &fenceInfo, NULL, &swapchainData.fences[i]);

        if (renderDevice.IsPresentQueueSeprate()) {
            vkCreateSemaphore(renderDevice.GetDevice(), &semaphoreInfo, NULL, &swapchainData.imageOwnershipSemaphores[i]);
        }

        if (renderDevice.IsTransferQueueSeprate()) {
            vkCreateSemaphore(renderDevice.GetDevice(), &semaphoreInfo, NULL, &swapchainData.transferSemaphores[i]);
        }
    }

    fenceInfo.flags = 0;
    vkCreateFence(renderDevice.GetDevice(), &fenceInfo, NULL, &swapchainData.transferSyncFence);
}

void Renderer::Swapchain::CreateSwapchainResources()
{
    this->CreateDepthResources();

    for (size_t i = 0; i < imagesCount; i++) {
        // Create Image view:
        swapchainData.swapChainImageViews[i] = CreateImageView(swapchainData.swapChainImages[i], swapchainData.swapChainImageFormat);

        // Create framebuffers:
        VkImageView attachments[] = {
            swapchainData.swapChainImageViews[i],
            swapchainData.depthStencilIamgeView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = swapchainData.swapChainExtent.width;
        framebufferInfo.height          = swapchainData.swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(renderDevice.GetDevice(), &framebufferInfo, NULL, &swapchainData.swapChainFramebuffers[i]) != VK_SUCCESS) {
            ASSERTF(true, "failed to create framebuffer!");
        }
    }
}

void Renderer::Swapchain::CreateSwapchainRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainData.swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount  = 2;
    renderPassInfo.pAttachments     = attachments;
    renderPassInfo.subpassCount     = 1;
    renderPassInfo.pSubpasses       = &subpass;
    renderPassInfo.dependencyCount  = 1;
    renderPassInfo.pDependencies    = &dependency;

    if (vkCreateRenderPass(renderDevice.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void Renderer::Swapchain::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();
    swapchainData.depthStencilImage = CreateImage(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    swapchainData.depthStencilImageMemory = renderDevice.AllocateDedicatedMemory(swapchainData.depthStencilImage); // alocate and bind memory
    swapchainData.depthStencilIamgeView = this->CreateImageView(swapchainData.depthStencilImage, depthFormat);
}

VkSurfaceFormatKHR Renderer::Swapchain::ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Renderer::Swapchain::ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {        
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent)
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

VkFramebuffer Renderer::Swapchain::GetCurrentFramebuffer() const 
{ 
    return swapchainData.swapChainFramebuffers[renderContext.GetCurrentImageIndex()]; 
}

VkFormat Renderer::Swapchain::FindSupportedFormat(const TRE::Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(renderDevice.GetGPU(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    ASSERTF(true, "Failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
}

VkFormat Renderer::Swapchain::FindDepthFormat()
{
    return this->FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkImageView Renderer::Swapchain::CreateImageView(VkImage image, VkFormat format)
{
    VkImageView outView;

    // Create Image view:
    VkImageViewCreateInfo createInfo{};
    createInfo.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image        = image;
    createInfo.viewType     = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format       = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask      = FormatToAspectMask(format);
    createInfo.subresourceRange.baseMipLevel    = 0;
    createInfo.subresourceRange.levelCount      = 1;
    createInfo.subresourceRange.baseArrayLayer  = 0;
    createInfo.subresourceRange.layerCount      = 1;

    if (vkCreateImageView(renderDevice.GetDevice(), &createInfo, NULL, &outView) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create image views!");
    }

    return outView;
}

VkImage Renderer::Swapchain::CreateImage(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    VkImage outImage;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = swapchainData.swapChainExtent.width;
    imageInfo.extent.height = swapchainData.swapChainExtent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(renderDevice.GetDevice(), &imageInfo, NULL, &outImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    return outImage;
}

/*void Renderer::Swapchain::BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex)
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
    imageOwnershipBarrier.image = ctx.swapchainData.swapChainImages[imageIndex];
    imageOwnershipBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(presentCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VkDependencyFlagBits{}, 0, NULL, 0, NULL, 1, &imageOwnershipBarrier);

    vkEndCommandBuffer(presentCommandBuffer);
}*/

/*void Renderer::Swapchain::CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx)
{
    const Internal::QueueFamilyIndices& queueFamilyIndices = renderDevice.GetQueueFamilyIndices();

    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[Internal::QFT_GRAPHICS];
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(renderDevice.GetDevice(), &poolInfo, NULL, &ctx.contextData.commandPool) != VK_SUCCESS) {
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

/*void Renderer::Swapchain::CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx)
{
    SwapchainData& swapchainData = ctx.swapchainData;
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

        for (uint32 i = 0; i < SwapchainData::MAX_FRAMES_IN_FLIGHT; i++) {
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
}*/

//Renderer::Internal::SwapchainSupportDetails Renderer::Internal::QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
//{
//    SwapchainSupportDetails details;
//
//    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
//
//    uint32_t formatCount;
//    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);
//
//    if (formatCount != 0) {
//        details.formats.Resize(formatCount);
//        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.Data());
//    }
//
//    uint32_t presentModeCount;
//    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);
//
//    if (presentModeCount != 0) {
//        details.presentModes.Resize(presentModeCount);
//        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.Data());
//    }
//
//    return details;
//}
//
//void Renderer::Internal::CreateSwapchain(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    ASSERT(renderDevice.gpu == VK_NULL_HANDLE);
//    ASSERT(ctx.surface == VK_NULL_HANDLE);
//
//    uint32 width = ctx.window->getSize().x;
//    uint32 height = ctx.window->getSize().y;
//    VkSwapchainKHR oldSwapchain = ctx.swapChain;
//
//    ctx.swapchainData.swapChainExtent        = VkExtent2D{ width, height };
//    
//    SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(renderDevice.gpu, ctx.surface);
//    VkSurfaceFormatKHR surfaceFormat         = ChooseSwapSurfaceFormat(swapChainSupport.formats);
//    VkPresentModeKHR presentMode             = ChooseSwapPresentMode(swapChainSupport.presentModes);
//    VkExtent2D extent                        = ChooseSwapExtent(swapChainSupport.capabilities, ctx.swapchainData.swapChainExtent);
//    ctx.imagesCount                          = swapChainSupport.capabilities.minImageCount + 1;
//
//    if (swapChainSupport.capabilities.maxImageCount > 0 && ctx.imagesCount > swapChainSupport.capabilities.maxImageCount) {
//        ctx.imagesCount = swapChainSupport.capabilities.maxImageCount;
//    }
//
//    VkSwapchainCreateInfoKHR    createInfo{};
//    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//    createInfo.surface          = ctx.surface;
//    createInfo.minImageCount    = ctx.imagesCount;
//    createInfo.imageFormat      = surfaceFormat.format;
//    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
//    createInfo.imageExtent      = extent;
//    createInfo.imageArrayLayers = 1;
//    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
//
//    const QueueFamilyIndices& indices = renderDevice.queueFamilyIndices;
//    uint32 queueFamilyIndices[]       = { indices.queueFamilies[QFT_GRAPHICS], indices.queueFamilies[QFT_PRESENT] };
//
//    if (indices.queueFamilies[QFT_GRAPHICS] != indices.queueFamilies[QFT_PRESENT]) {
//        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
//        createInfo.queueFamilyIndexCount = 2;
//        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
//    } else {
//        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
//    }
//
//    createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
//    createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//    createInfo.presentMode      = presentMode;
//    createInfo.clipped          = VK_TRUE;
//    createInfo.oldSwapchain     = oldSwapchain;
//
//    if (vkCreateSwapchainKHR(renderDevice.device, &createInfo, NULL, &ctx.swapChain) != VK_SUCCESS) {
//        ASSERTF(true, "Failed to create swap chain!");
//    }
//
//    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &ctx.imagesCount, NULL);
//    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &ctx.imagesCount, ctx.swapchainData.swapChainImages);
//    
//    ctx.swapchainData.swapChainImageFormat = surfaceFormat.format;
//    ctx.swapchainData.swapChainExtent      = extent;
//
//    if (oldSwapchain == VK_NULL_HANDLE) {
//        CreateSyncObjects(renderDevice, ctx);
//        CreateSwapchainRenderPass(renderDevice, ctx);
//        CreateCommandPool(renderDevice, ctx);
//        CreateCommandBuffers(renderDevice, ctx);
//    } else {
//        vkDestroySwapchainKHR(renderDevice.device, oldSwapchain, NULL);
//    }
//
//    CreateSwapchainResources(renderDevice, ctx);
//}
//
//void Renderer::Internal::DestroySwapchain(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    CleanupSwapchain(ctx, renderDevice);
//
//    vkDestroySwapchainKHR(renderDevice.device, ctx.swapChain, NULL);
//    ctx.swapChain = VK_NULL_HANDLE;
//
//    for (size_t i = 0; i < SwapchainData::MAX_FRAMES_IN_FLIGHT; i++) {
//        vkDestroySemaphore(renderDevice.device, ctx.swapchainData.drawCompleteSemaphores[i], NULL);
//        vkDestroySemaphore(renderDevice.device, ctx.swapchainData.imageAcquiredSemaphores[i], NULL);
//        vkDestroyFence(renderDevice.device, ctx.swapchainData.fences[i], NULL);
//    }
//
//    vkDestroyCommandPool(renderDevice.device, ctx.contextData.commandPool, NULL);
//}
//
//void Renderer::Internal::CleanupSwapchain(RenderContext& ctx, const RenderDevice& renderDevice)
//{
//    SwapchainData& swapchainData   = ctx.swapchainData;
//    VkDevice device                 = renderDevice.device;
//
//    for (size_t i = 0; i < ctx.imagesCount; i++) {
//        vkDestroyFramebuffer(device, swapchainData.swapChainFramebuffers[i], NULL);
//        vkDestroyImageView(device, swapchainData.swapChainImageViews[i], NULL);
//    }
//
//    // vkFreeCommandBuffers(device, swapchainData.commandPool, static_cast<uint32_t>(swapchainData.commandBuffers.Size()), swapchainData.commandBuffers.Data());
//
//    // vkDestroyPipeline(device, swapchainData.graphicsPipeline, NULL);
//    // vkDestroyPipelineLayout(device, swapchainData.pipelineLayout, NULL);
//    // vkDestroyRenderPass(device, swapchainData.renderPass, NULL);
//}
//
//void Renderer::Internal::RecreateSwapchainInternal(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    vkDeviceWaitIdle(renderDevice.device);
//    CleanupSwapchain(ctx, renderDevice);
//
//    uint32 width = ctx.window->getSize().x;
//    uint32 height = ctx.window->getSize().y;
//
//    while (width == 0 || height == 0) {
//        ctx.window->WaitEvents();
//
//        width = ctx.window->getSize().x;
//        height = ctx.window->getSize().y;
//    }
//
//    // ctx.framebufferResized = false;
//    CreateSwapchain(renderDevice, ctx);
//}
//
//void Renderer::Internal::UpdateSwapchain(RenderContext& ctx)
//{
//    ctx.framebufferResized = true;
//    // RecreateSwapchainInternal(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
//}
//
//
//void Renderer::Internal::CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    ctx.currentFrame = 0;
//
//    VkSemaphoreCreateInfo semaphoreInfo{};
//    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//    VkFenceCreateInfo fenceInfo{};
//    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//    for (size_t i = 0; i < SwapchainData::MAX_FRAMES_IN_FLIGHT; i++) {
//        vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapchainData.imageAcquiredSemaphores[i]);
//        vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapchainData.drawCompleteSemaphores[i]);
//        vkCreateFence(renderDevice.device, &fenceInfo, NULL, &ctx.swapchainData.fences[i]);
//
//        if (renderDevice.isPresentQueueSeprate) {
//            vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapchainData.imageOwnershipSemaphores[i]);
//        }
//
//        if (renderDevice.isTransferQueueSeprate) {
//            vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapchainData.transferSemaphores[i]);
//        }
//    }
//
//    fenceInfo.flags = 0;
//    vkCreateFence(renderDevice.device, &fenceInfo, NULL, &ctx.swapchainData.transferSyncFence);
//}
//
//void Renderer::Internal::CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    const QueueFamilyIndices& queueFamilyIndices = renderDevice.queueFamilyIndices;
//
//    {
//        VkCommandPoolCreateInfo poolInfo{};
//        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_GRAPHICS];
//        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//
//        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.commandPool) != VK_SUCCESS) {
//            ASSERTF(true, "failed to create command pool!");
//        }
//    }
//
//    {
//        VkCommandPoolCreateInfo poolInfo{};
//        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_TRANSFER];
//        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//
//        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.memoryCommandPool) != VK_SUCCESS) {
//            ASSERTF(true, "failed to create command pool!");
//        }
//    }
//
//    if (renderDevice.isPresentQueueSeprate) {
//        VkCommandPoolCreateInfo poolInfo{};
//        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_PRESENT];
//        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//
//        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.contextData.presentCommandPool) != VK_SUCCESS) {
//            ASSERTF(true, "failed to create command pool!");
//        }
//    }
//}
//
//void Renderer::Internal::CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    SwapchainData& swapchainData = ctx.swapchainData;
//    RenderContextData& ctxData = ctx.contextData;
//
//    {
//        VkCommandBufferAllocateInfo allocInfo[3] = { {} };
//        allocInfo[0].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//        allocInfo[0].commandPool           = ctxData.commandPool;
//        allocInfo[0].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//        allocInfo[0].commandBufferCount    = 1;
//
//        allocInfo[1].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//        allocInfo[1].commandPool           = ctxData.memoryCommandPool;
//        allocInfo[1].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//        allocInfo[1].commandBufferCount    = 1;
//
//        allocInfo[2].sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//        allocInfo[2].commandPool           = ctxData.presentCommandPool;
//        allocInfo[2].level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//        allocInfo[2].commandBufferCount    = 1;
//
//        for (uint32 i = 0; i < SwapchainData::MAX_FRAMES_IN_FLIGHT; i++) {
//            if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[0], &ctxData.contextFrameResources[i].graphicsCommandBuffer) != VK_SUCCESS) {
//                ASSERTF(true, "failed to allocate command buffers!");
//            }
//
//            if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[1], &ctxData.contextFrameResources[i].transferCommandBuffer) != VK_SUCCESS) {
//                ASSERTF(true, "failed to allocate command buffers!");
//            }
//
//            if (renderDevice.isPresentQueueSeprate) {
//                if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo[2], &ctxData.contextFrameResources[i].presentCommandBuffer) != VK_SUCCESS) {
//                    ASSERTF(true, "failed to allocate command buffers!");
//                }
//
//                BuildImageOwnershipCmd(renderDevice, ctx, i);
//            }
//        }
//    }
//}
//
//void Renderer::Internal::CreateSwapchainResources(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    SwapchainData& swapchainData = ctx.swapchainData;
//
//    for (size_t i = 0; i < ctx.imagesCount; i++) {
//        // Create Image view:
//        VkImageViewCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//        createInfo.image = swapchainData.swapChainImages[i];
//        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//        createInfo.format = swapchainData.swapChainImageFormat;
//        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        createInfo.subresourceRange.baseMipLevel = 0;
//        createInfo.subresourceRange.levelCount = 1;
//        createInfo.subresourceRange.baseArrayLayer = 0;
//        createInfo.subresourceRange.layerCount = 1;
//
//        if (vkCreateImageView(renderDevice.device, &createInfo, NULL, &swapchainData.swapChainImageViews[i]) != VK_SUCCESS) {
//            ASSERTF(true, "Failed to create image views!");
//        }
//
//        // Create framebuffers:
//        VkImageView attachments[] = {
//            swapchainData.swapChainImageViews[i]
//        };
//
//        VkFramebufferCreateInfo framebufferInfo{};
//        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//        framebufferInfo.renderPass = ctx.contextData.renderPass;
//        framebufferInfo.attachmentCount = 1;
//        framebufferInfo.pAttachments = attachments;
//        framebufferInfo.width = ctx.swapchainData.swapChainExtent.width;
//        framebufferInfo.height = ctx.swapchainData.swapChainExtent.height;
//        framebufferInfo.layers = 1;
//
//        if (vkCreateFramebuffer(renderDevice.device, &framebufferInfo, NULL, &swapchainData.swapChainFramebuffers[i]) != VK_SUCCESS) {
//            ASSERTF(true, "failed to create framebuffer!");
//        }
//    }
//}
//
//void Renderer::Internal::CreateSwapchainRenderPass(const RenderDevice& renderDevice, RenderContext& ctx)
//{
//    RenderPassDesc renderpassDesc;
//
//    VkAttachmentDescription colorAttachment{};
//    colorAttachment.format          = ctx.swapchainData.swapChainImageFormat;
//    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
//    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
//    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
//    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//
//    VkSubpassDependency dependency{};
//    dependency.srcSubpass       = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass       = 0;
//    dependency.srcStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.srcAccessMask    = 0;
//    dependency.dstStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//
//    VkAttachmentReference colorAttachmentRef{};
//    colorAttachmentRef.attachment   = 0;
//    colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    VkSubpassDescription subpass{};
//    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
//    subpass.colorAttachmentCount    = 1;
//    subpass.pColorAttachments       = &colorAttachmentRef;
//
//    renderpassDesc.attachments.EmplaceBack(colorAttachment);
//    renderpassDesc.subpassDependency.EmplaceBack(dependency);
//    renderpassDesc.subpassesDesc.EmplaceBack(subpass);
//
//    CreateRenderPass(renderDevice, &ctx.contextData.renderPass, renderpassDesc);
//}
//
//void Renderer::Internal::BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex)
//{
//    VkCommandBuffer presentCommandBuffer = ctx.contextData.contextFrameResources[imageIndex].presentCommandBuffer;
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//
//    vkBeginCommandBuffer(presentCommandBuffer, &beginInfo);
//
//    VkImageMemoryBarrier imageOwnershipBarrier{};
//    imageOwnershipBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//    imageOwnershipBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//    imageOwnershipBarrier.srcQueueFamilyIndex = renderDevice.queueFamilyIndices.queueFamilies[QFT_GRAPHICS];
//    imageOwnershipBarrier.dstQueueFamilyIndex = renderDevice.queueFamilyIndices.queueFamilies[QFT_PRESENT];
//    imageOwnershipBarrier.image = ctx.swapchainData.swapChainImages[imageIndex];
//    imageOwnershipBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
//
//    vkCmdPipelineBarrier(presentCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
//        VkDependencyFlagBits{}, 0, NULL, 0, NULL, 1, &imageOwnershipBarrier);
//
//    vkEndCommandBuffer(presentCommandBuffer);
//}
//
//VkSurfaceFormatKHR Renderer::Internal::ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats) 
//{
//    for (const auto& availableFormat : availableFormats) {
//        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//            return availableFormat;
//        }
//    }
//
//    return availableFormats[0];
//}
//
//VkPresentModeKHR Renderer::Internal::ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes)
//{
//    for (const auto& availablePresentMode : availablePresentModes) {        
//        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//            return availablePresentMode;
//        }
//    }
//    
//    return VK_PRESENT_MODE_FIFO_KHR;
//}
//
//VkExtent2D Renderer::Internal::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent)
//{
//    if (capabilities.currentExtent.width != UINT32_MAX) {
//        return capabilities.currentExtent;
//    } else {
//        VkExtent2D actualExtent = extent;
//
//        actualExtent.width = TRE::Math::Max<uint32>(
//            capabilities.minImageExtent.width, 
//            TRE::Math::Min<uint32>(capabilities.maxImageExtent.width, actualExtent.width)
//        );
//        actualExtent.height = TRE::Math::Max<uint32>(
//            capabilities.minImageExtent.height, 
//            TRE::Math::Min<uint32>(capabilities.maxImageExtent.height, actualExtent.height)
//        );
//
//        return actualExtent;
//    }
//}

TRE_NS_END
