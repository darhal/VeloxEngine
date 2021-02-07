#include "Swapchain.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::Swapchain::Swapchain(RenderBackend& backend) :
    renderBackend(backend)
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
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    RenderContext& renderContext = renderBackend.GetRenderContext();

    ASSERT(renderBackend.GetRenderDevice().GetDevice() == VK_NULL_HANDLE);
    ASSERT(renderBackend.GetRenderContext().GetSurface() == VK_NULL_HANDLE);

    uint32 width = renderContext.GetWindow()->getSize().x;
    uint32 height = renderContext.GetWindow()->getSize().y;
    VkSwapchainKHR oldSwapchain = swapchain;

    swapchainData.swapChainExtent    = VkExtent2D{ width, height };
    supportDetails                   = QuerySwapchainSupport(renderDevice.GetGPU(), renderContext.GetSurface());
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode     = ChooseSwapPresentMode(supportDetails.presentModes);
    VkExtent2D extent                = ChooseSwapExtent(supportDetails.capabilities, swapchainData.swapChainExtent);
    imagesCount                      = supportDetails.capabilities.minImageCount + 1;

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
        // CreateSwapchainRenderPass();
        // CreateCommandPool(renderDevice, ctx);
        // CreateCommandBuffers(renderDevice, ctx);
    } else {
        vkDestroySwapchainKHR(renderDevice.GetDevice(), oldSwapchain, NULL);
    }

    // CreateSwapchainResources();
    CreateSwapchainResources(swapchainData.swapChainImages);
}

void Renderer::Swapchain::DestroySwapchain()
{
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    RenderContext& renderContext = renderBackend.GetRenderContext();

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
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    const auto& renderContext = renderBackend.GetRenderContext();
    VkDevice device = renderDevice.GetDevice();

    /*for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyFramebuffer(device, swapchainData.swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device, swapchainData.swapChainImageViews[i], NULL);
    }

    vkDestroyImageView(device, swapchainData.depthStencilIamgeView, NULL);
    vkDestroyImage(device, swapchainData.depthStencilImage, NULL);
    renderDevice.FreeDedicatedMemory(swapchainData.depthStencilImageMemory);*/
}

void Renderer::Swapchain::RecreateSwapchain()
{
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    RenderContext& renderContext = renderBackend.GetRenderContext();

    vkDeviceWaitIdle(renderDevice.GetDevice());
    CleanupSwapchain();

    uint32 width = renderContext.GetWindow()->getSize().x;
    uint32 height = renderContext.GetWindow()->getSize().y;

    while (width == 0 || height == 0) {
        renderContext.GetWindow()->WaitEvents();

        width = renderContext.GetWindow()->getSize().x;
        height = renderContext.GetWindow()->getSize().y;
    }

    framebufferResized = false;
    CreateSwapchain();
}

void Renderer::Swapchain::QueueSwapchainUpdate()
{
    framebufferResized = true;
    // RecreateSwapchainInternal(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
}

void Renderer::Swapchain::CreateSyncObjects()
{
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    RenderContext& renderContext = renderBackend.GetRenderContext();

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
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();

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

void Renderer::Swapchain::CreateSwapchainResources(const VkImage* images)
{
    VkFormat format = swapchainData.swapChainImageFormat;
    const auto info = ImageCreateInfo::RenderTarget(swapchainData.swapChainExtent.width, swapchainData.swapChainExtent.height, format);

    for (uint32 i = 0; i < imagesCount; i++) {
        VkImageViewCreateInfo view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        view_info.image = images[i];
        view_info.format = swapchainData.swapChainImageFormat;
        view_info.components.r = VK_COMPONENT_SWIZZLE_R;
        view_info.components.g = VK_COMPONENT_SWIZZLE_G;
        view_info.components.b = VK_COMPONENT_SWIZZLE_B;
        view_info.components.a = VK_COMPONENT_SWIZZLE_A;
        view_info.subresourceRange.aspectMask = FormatToAspectMask(format);
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.layerCount = 1;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

        VkImageView image_view;
        vkCreateImageView(renderBackend.GetRenderDevice().GetDevice(), &view_info, NULL, &image_view);

        swapchainData.swapchainImages[i] = ImageHandle(renderBackend.objectsPool.images.Allocate(renderBackend, images[i], image_view, info, VK_IMAGE_VIEW_TYPE_2D));
        swapchainData.swapchainImages[i]->SetSwapchainLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        // printf("Image view handle: %llu\n", swapchainData.swapchainImages[i]);
    }
}

Renderer::ImageHandle Renderer::Swapchain::GetSwapchainImage(uint32 i)
{
    return swapchainData.swapchainImages[i];
}

void Renderer::Swapchain::CreateSwapchainRenderPass()
{
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindSupportedDepthStencilFormat();
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
    VkFormat depthFormat = FindSupportedDepthStencilFormat();
    swapchainData.depthStencilImage = CreateImage(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    swapchainData.depthStencilImageMemory = renderBackend.GetRenderDevice().AllocateDedicatedMemory(swapchainData.depthStencilImage); // alocate and bind memory
    swapchainData.depthStencilIamgeView = this->CreateImageView(swapchainData.depthStencilImage, depthFormat);
}

VkSurfaceFormatKHR Renderer::Swapchain::ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
    RenderContext& renderContext = renderBackend.GetRenderContext();
    return swapchainData.swapChainFramebuffers[renderContext.GetCurrentImageIndex()]; 
}

VkFormat Renderer::Swapchain::FindSupportedFormat(const std::initializer_list<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(renderBackend.GetRenderDevice().GetGPU(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    ASSERTF(true, "Failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
}

VkFormat Renderer::Swapchain::FindSupportedDepthStencilFormat()
{
    return this->FindSupportedFormat(
        { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkFormat Renderer::Swapchain::FindSupportedDepthFormat()
{
    return this->FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM },
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

    if (vkCreateImageView(renderBackend.GetRenderDevice().GetDevice(), &createInfo, NULL, &outView) != VK_SUCCESS) {
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

    if (vkCreateImage(renderBackend.GetRenderDevice().GetDevice(), &imageInfo, NULL, &outImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    return outImage;
}

TRE_NS_END
