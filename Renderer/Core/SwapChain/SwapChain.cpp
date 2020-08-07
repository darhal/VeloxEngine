#include "SwapChain.hpp"
#include <Renderer/Core/Common/Utils.hpp>
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

void Renderer::CreateSwapChain(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice)
{
    ASSERT(renderDevice.gpu == VK_NULL_HANDLE);
    ASSERT(ctx.surface == VK_NULL_HANDLE);
 
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(renderDevice.gpu, ctx.surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode     = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent                = ChooseSwapExtent(swapChainSupport.capabilities, ctx.swapChainData.swapChainExtent);

    uint32 imageCount                = swapChainSupport.capabilities.minImageCount;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR    createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = ctx.surface;
    createInfo.minImageCount    = imageCount;
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
    createInfo.oldSwapchain     = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(renderDevice.device, &createInfo, NULL, &ctx.swapChain) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &imageCount, NULL);
    ctx.swapChainData.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(renderDevice.device, ctx.swapChain, &imageCount, ctx.swapChainData.swapChainImages.data());

    ctx.swapChainData.swapChainImageFormat = surfaceFormat.format;
    ctx.swapChainData.swapChainExtent = extent;

    CreateImageViews(renderDevice, ctx);

    createRenderPass(renderDevice, ctx);
    createGraphicsPipeline(renderDevice, ctx);
    createFrameBuffers(renderDevice, ctx);

    CreateSyncObjects(renderDevice, ctx);
    CreateCommandPool(renderDevice, ctx);
    CreateCommandBuffers(renderDevice, ctx);
}

void Renderer::DestroySwapChain(const RenderDevice& renderDevice, RenderContext& ctx)
{
    for (size_t i = 0; i < SwapChainData::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(renderDevice.device, ctx.swapChainData.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(renderDevice.device, ctx.swapChainData.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(renderDevice.device, ctx.swapChainData.inFlightFences[i], nullptr);
    }

    for (auto imageView : ctx.swapChainData.swapChainImageViews) {
        vkDestroyImageView(renderDevice.device, imageView, NULL);
    }

    vkDestroySwapchainKHR(renderDevice.device, ctx.swapChain, NULL);
}

void Renderer::Present(RenderEngine& engine, const TRE::Vector<VkCommandBuffer>& cmdbuff)
{
    RenderContext& ctx = *engine.renderContext;
    RenderDevice& renderDevice = *engine.renderDevice;
    SwapChainData& swapChainData = engine.renderContext->swapChainData;
    VkDevice device = renderDevice.device;
    uint32 currentFrame = swapChainData.currentFrame;

    vkWaitForFences(device, 1, &engine.renderContext->swapChainData.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(device, ctx.swapChain, UINT64_MAX, swapChainData.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (swapChainData.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &swapChainData.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    swapChainData.imagesInFlight[imageIndex] = swapChainData.inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { swapChainData.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1; // (uint32)cmdbuff.Size();
    submitInfo.pCommandBuffers = &swapChainData.commandBuffers[currentFrame]; // cmdbuff.Data();

    VkSemaphore signalSemaphores[] = { swapChainData.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &swapChainData.inFlightFences[currentFrame]);

    if (vkQueueSubmit(renderDevice.queues[QFT_GRAPHICS], 1, &submitInfo, swapChainData.inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Presenting:
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { ctx.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(renderDevice.queues[QFT_PRESENT], &presentInfo);

    swapChainData.currentFrame = (currentFrame + 1) % SwapChainData::MAX_FRAMES_IN_FLIGHT;
    // printf("Present! %d\n", imageIndex);
}

void Renderer::CreateImageViews(const RenderDevice& renderDevice, RenderContext& ctx)
{
    ctx.swapChainData.swapChainImageViews.Resize(ctx.swapChainData.swapChainImages.size());

    for (size_t i = 0; i < ctx.swapChainData.swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = ctx.swapChainData.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = ctx.swapChainData.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderDevice.device, &createInfo, NULL, &ctx.swapChainData.swapChainImageViews[i]) != VK_SUCCESS) {
            ASSERTF(true, "Failed to create image views!");
        }
    }
}

void Renderer::CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx)
{
    ctx.swapChainData.currentFrame = 0;

    ctx.swapChainData.imageAvailableSemaphores.resize(SwapChainData::MAX_FRAMES_IN_FLIGHT);
    ctx.swapChainData.renderFinishedSemaphores.resize(SwapChainData::MAX_FRAMES_IN_FLIGHT);
    ctx.swapChainData.inFlightFences.resize(SwapChainData::MAX_FRAMES_IN_FLIGHT);
    ctx.swapChainData.imagesInFlight.resize(ctx.swapChainData.swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < SwapChainData::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(renderDevice.device, &semaphoreInfo, NULL, &ctx.swapChainData.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(renderDevice.device, &fenceInfo, NULL, &ctx.swapChainData.inFlightFences[i]) != VK_SUCCESS) {
            
            ASSERTF(true, "failed to create synchronization objects for a frame!");
        }
    }
}

void Renderer::CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx)
{
    const QueueFamilyIndices& queueFamilyIndices = renderDevice.queueFamilyIndices;

    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.queueFamilies[QFT_GRAPHICS];

        if (vkCreateCommandPool(renderDevice.device, &poolInfo, NULL, &ctx.swapChainData.commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }
}

void Renderer::CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx)
{
    SwapChainData& swapChainData = ctx.swapChainData;

    swapChainData.commandBuffers.Resize(swapChainData.MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = swapChainData.commandPool;
    allocInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)swapChainData.commandBuffers.Size();

    if (vkAllocateCommandBuffers(renderDevice.device, &allocInfo, swapChainData.commandBuffers.Data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkClearColorValue clearColor = { 164.0f / 256.0f, 30.0f / 256.0f, 34.0f / 256.0f, 0.0f };
    VkClearValue clearValue = {};
    clearValue.color = clearColor;

    VkImageSubresourceRange imageRange = {};
    imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageRange.levelCount = 1;
    imageRange.layerCount = 1;

    for (size_t i = 0; i < swapChainData.commandBuffers.Size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if (vkBeginCommandBuffer(swapChainData.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = swapChainData.renderPass;
        renderPassInfo.framebuffer       = swapChainData.swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainData.swapChainExtent;
        renderPassInfo.clearValueCount   = 1;
        renderPassInfo.pClearValues      = &clearValue;

        vkCmdBeginRenderPass(swapChainData.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(swapChainData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, swapChainData.graphicsPipeline);
        vkCmdDraw(swapChainData.commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(swapChainData.commandBuffers[i]);

        // vkCmdClearColorImage(swapChain.commandBuffers[i], swapChain.swapChainImages[i], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);

        if (vkEndCommandBuffer(swapChainData.commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void Renderer::createGraphicsPipeline(const RenderDevice& renderDevice, RenderContext& ctx)
{
    auto vertShaderCode = ReadShaderFile("Shaders/vert.spv");
    auto fragShaderCode = ReadShaderFile("Shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(renderDevice.device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(renderDevice.device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage   = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module  = vertShaderModule;
    vertShaderStageInfo.pName   = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module  = fragShaderModule;
    fragShaderStageInfo.pName   = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 0;
    vertexInputInfo.pVertexBindingDescriptions      = NULL; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions    = NULL; // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)ctx.swapChainData.swapChainExtent.width;
    viewport.height = (float)ctx.swapChainData.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = ctx.swapChainData.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;


    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp          = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional



    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f; // Optional
    multisampling.pSampleMask           = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable      = VK_FALSE; // Optional


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable            = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional


    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount  = 2;
    dynamicState.pDynamicStates     = dynamicStates;


    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount       = 0; // Optional
    pipelineLayoutInfo.pSetLayouts          = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges  = nullptr; // Optional

    if (vkCreatePipelineLayout(renderDevice.device, &pipelineLayoutInfo, nullptr, &ctx.swapChainData.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages    = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = NULL; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = NULL; // Optional
    pipelineInfo.layout = ctx.swapChainData.pipelineLayout;
    pipelineInfo.renderPass = ctx.swapChainData.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(renderDevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &ctx.swapChainData.graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(renderDevice.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderDevice.device, vertShaderModule, nullptr);
}

void Renderer::createRenderPass(const RenderDevice& renderDevice, RenderContext& ctx)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format          = ctx.swapChainData.swapChainImageFormat;
    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment   = 0;
    colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    if (vkCreateRenderPass(renderDevice.device, &renderPassInfo, nullptr, &ctx.swapChainData.renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void Renderer::createFrameBuffers(const RenderDevice& renderDevice, RenderContext& ctx)
{
    ctx.swapChainData.swapChainFramebuffers.resize(ctx.swapChainData.swapChainImageViews.Size());

    for (size_t i = 0; i < ctx.swapChainData.swapChainImageViews.Size(); i++) {
        VkImageView attachments[] = {
            ctx.swapChainData.swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = ctx.swapChainData.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = ctx.swapChainData.swapChainExtent.width;
        framebufferInfo.height = ctx.swapChainData.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(renderDevice.device, &framebufferInfo, nullptr, &ctx.swapChainData.swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

VkShaderModule Renderer::createShaderModule(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
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
