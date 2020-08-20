#include "pch.hpp"

#if not defined(BUILD_EXEC)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#else

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/Renderer.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/Buffer/Buffer.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Core/Misc/Maths/Maths.hpp>
#include <Engine/Core/Misc/Utils/Logging.hpp>

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

std::vector<TRE::Renderer::Internal::Vertex> vertices = {
    {TRE::vec3{-0.5f, -0.5f, 0.f},  TRE::vec3{1.0f, 0.0f, 0.0f}},
    {TRE::vec3{0.5f, -0.5f, 0.f},   TRE::vec3{0.0f, 1.0f, 0.0f}},
    {TRE::vec3{0.5f, 0.5f, 0.f},    TRE::vec3{0.0f, 0.0f, 1.0f}},
    {TRE::vec3{-0.5f, 0.5f, 0.f},   TRE::vec3{1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

void updateMVP(const TRE::Renderer::Internal::RenderDevice& dev, const TRE::Renderer::Internal::RenderContext& ctx, TRE::Renderer::Buffer& buffer)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    const TRE::Renderer::Internal::SwapChainData& swapData = ctx.swapChainData;

    MVP mvp{};

    mvp.model   = glm::rotate(glm::mat4(1.0f), time * TRE::Math::ToRad(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.view    = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj    = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapData.swapChainExtent.width / (float)swapData.swapChainExtent.height, 0.1, 10.f);
    mvp.proj[1][1] *= -1;
    
    //mvp.model.transpose();
    //mvp.view.transpose();
    //mvp.proj.transpose();
   
    buffer.WriteToBuffer(sizeof(mvp), &mvp);
}

void InitRenderPassDesc(const TRE::Renderer::Internal::RenderDevice& dev, const TRE::Renderer::Internal::RenderContext& ctx, TRE::Renderer::Internal::RenderPassDesc& desc)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = ctx.swapChainData.swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    desc.attachments.EmplaceBack(colorAttachment);
    desc.subpassDependency.EmplaceBack(dependency);
    desc.subpassesDesc.EmplaceBack(subpass);
}

void InitGraphicsPipelineDesc(const TRE::Renderer::Internal::RenderDevice& dev, const TRE::Renderer::Internal::RenderContext& ctx, TRE::Renderer::Internal::GraphicsPiplineDesc& desc)
{
    auto vertShaderCode = TRE::Renderer::ReadShaderFile("shaders/vert.spv");
    auto fragShaderCode = TRE::Renderer::ReadShaderFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = TRE::Renderer::Internal::CreateShaderModule(dev.device, vertShaderCode);
    VkShaderModule fragShaderModule = TRE::Renderer::Internal::CreateShaderModule(dev.device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderInfo{};
    vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderInfo.module = vertShaderModule;
    vertexShaderInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    desc.shaderStagesDesc.EmplaceBack(vertexShaderInfo);
    desc.shaderStagesDesc.EmplaceBack(fragShaderStageInfo);

    TRE::Renderer::Internal::VertexInputDesc vertexInputDesc;
    vertexInputDesc.binding = 0;
    vertexInputDesc.stride = sizeof(TRE::Renderer::Internal::Vertex);
    vertexInputDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputDesc.attribDesc.EmplaceBack(TRE::Renderer::Internal::VertextAttribDesc{ 0, offsetof(TRE::Renderer::Internal::Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT });
    vertexInputDesc.attribDesc.EmplaceBack(TRE::Renderer::Internal::VertextAttribDesc{ 1, offsetof(TRE::Renderer::Internal::Vertex, color), VK_FORMAT_R32G32B32_SFLOAT });
    desc.vertexInputDesc.EmplaceBack(vertexInputDesc);


    desc.inputAssemblyDesc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    desc.inputAssemblyDesc.primitiveRestartEnable = VK_FALSE;

    desc.rasterStateDesc.depthClampEnable = VK_FALSE;
    desc.rasterStateDesc.rasterizerDiscardEnable = VK_FALSE;
    desc.rasterStateDesc.polygonMode = VK_POLYGON_MODE_FILL;
    desc.rasterStateDesc.lineWidth = 1.0f;
    desc.rasterStateDesc.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
    desc.rasterStateDesc.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    desc.rasterStateDesc.depthBiasEnable = VK_FALSE;


    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding                = 0;
    uboLayoutBinding.descriptorType         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount        = 1;
    uboLayoutBinding.stageFlags             = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers     = NULL; // Optional
    
    VkDescriptorSetLayoutCreateInfo  layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(dev.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    desc.piplineLayoutDesc.descriptorSetLayout.EmplaceBack(descriptorSetLayout);

    desc.viewport.x = 0.0f;
    desc.viewport.y = 0.0f;
    desc.viewport.width = (float)ctx.swapChainData.swapChainExtent.width;
    desc.viewport.height = (float)ctx.swapChainData.swapChainExtent.height;
    desc.viewport.minDepth = 0.0f;
    desc.viewport.maxDepth = 1.0f;

    desc.scissor.offset = { 0, 0 };
    desc.scissor.extent = ctx.swapChainData.swapChainExtent;

    desc.subpass = 0;
    desc.basePipelineHandle = 0;
    desc.basePipelineIndex = -1;
}

void RenderFrame(uint32 i, const TRE::Renderer::Internal::RenderDevice& renderDevice, const TRE::Renderer::RenderContext& ctx, TRE::Renderer::Internal::GraphicsPipeline& graphicsPipeline, TRE::Renderer::Buffer& vertexIndexBuffer, const std::vector<VkDescriptorSet>& descriptorSets)
{
    const TRE::Renderer::Internal::SwapChainData& swapChainData = ctx.GetSwapChainData();
    // const TRE::Renderer::Internal::RenderContextData& ctxData = ctx.contextData;
    const TRE::Renderer::Internal::ContextFrameResources& ctxResource = ctx.GetFrameResource(i);

    VkDevice device = renderDevice.device;    
    VkCommandBuffer currentCmdBuff = ctxResource.graphicsCommandBuffer;

    vkResetCommandBuffer(currentCmdBuff, 0);

    VkClearColorValue clearColor = { 164.0f / 256.0f, 30.0f / 256.0f, 34.0f / 256.0f, 0.0f };
    VkClearValue clearValue = {};
    clearValue.color = clearColor;

    VkViewport viewport{};
    viewport.width      = (float)swapChainData.swapChainExtent.width;
    viewport.height     = (float)swapChainData.swapChainExtent.height;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainData.swapChainExtent;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;


    if (vkBeginCommandBuffer(currentCmdBuff, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    /*if (!renderDevice.isTransferQueueSeprate && ctxData.transferRequests) {
        VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

        vkCmdPipelineBarrier(currentCmdBuff,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT,
            1, &memoryBarrier, 0, NULL, 0, NULL
        );
    }*/

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType                = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass           = graphicsPipeline.renderPass;
    renderPassInfo.framebuffer          = swapChainData.swapChainFramebuffers[ctx.GetCurrentImageIndex()];
    renderPassInfo.renderArea.offset    = { 0, 0 };
    renderPassInfo.renderArea.extent    = swapChainData.swapChainExtent;
    renderPassInfo.clearValueCount      = 1;
    renderPassInfo.pClearValues         = &clearValue;

    vkCmdBeginRenderPass(currentCmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.pipeline);

    vkCmdSetViewport(currentCmdBuff, 0, 1, &viewport);
    vkCmdSetScissor(currentCmdBuff, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { vertexIndexBuffer.GetAPIObject() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(currentCmdBuff, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(currentCmdBuff, vertexIndexBuffer.GetAPIObject(), sizeof(vertices[0]) * vertices.size(), VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(currentCmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.pipelineLayout, 0, 1, &descriptorSets[0/*ctx.GetCurrentImageIndex()*/], 0, NULL);

    // vkCmdDraw(currentCmdBuff, 3, 1, 0, 0);
    
    vkCmdDrawIndexed(currentCmdBuff, 6, 1, 0, 0, 0);   
    
    vkCmdEndRenderPass(currentCmdBuff);

    //VkImageSubresourceRange imgRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    //vkCmdClearColorImage(currentCmdBuff, swapChainData.swapChainImages[currentBuffer], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imgRange);

    if (vkEndCommandBuffer(currentCmdBuff) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}



void printFPS() {
    static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
    static int fps;

    fps++;

    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
        oldTime = std::chrono::high_resolution_clock::now();
        std::cout << "FPS: " << fps << std::endl;
        fps = 0;
    }
}

int main()
{
    /*uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    std::vector<const char*> extensionsNames(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    int i = 0;
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
        extensionsNames[i++] = extension.extensionName;
    }*/

    const unsigned int SCR_WIDTH = 1920 / 2;
    const unsigned int SCR_HEIGHT = 1080 / 2;

    using namespace TRE::Renderer;
    using namespace TRE;

    Event ev;
    Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderEngine engine{ &window };

    Internal::RenderContext& ctx = engine.GetCtxInternal();
    Internal::RenderDevice& renderDevice = engine.GetDevInternal();
    Internal::SwapChainData& swapChainData = ctx.swapChainData;


    size_t vertexSize = sizeof(vertices[0]) * vertices.size();
    size_t indexSize = sizeof(indices[0]) * indices.size();

    uint32 queueFamilies = QueueFamilyFlag::NONE;

    if (renderDevice.isTransferQueueSeprate){
        queueFamilies = QueueFamilyFlag::TRANSFER | QueueFamilyFlag::GRAPHICS;
    }

    char* data = new char[vertexSize + indexSize];
    memcpy(data, vertices.data(), vertexSize);
    memcpy(data + vertexSize, indices.data(), indexSize);

    //Buffer staginVertexIndexBuffer = engine.GetRenderContext().CreateStagingBuffer(vertexSize + indexSize, data);
    Buffer vertexIndexBuffer =
        engine.GetRenderContext().CreateBuffer(vertexSize + indexSize, NULL,
            BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER | BufferUsage::INDEX_BUFFER,
            MemoryProperty::DEVICE_LOCAL, queueFamilies
        );

    engine.GetRenderContext().GetStagingManager().Stage(vertexIndexBuffer.GetAPIObject(), (void*)data, vertexSize + indexSize);
    // engine.GetRenderContext().GetStagingManager().Flush();
    // engine.GetRenderContext().GetStagingManager().Wait(currentStageBuffer);
    // Internal::TransferBufferInfo transferInfo;
    // transferInfo.srcBuffer = &staginVertexIndexBuffer;
    // transferInfo.dstBuffer = &vertexIndexBuffer;
    // transferInfo.copyRegions.EmplaceBack(VkBufferCopy{ 0, 0, vertexSize + indexSize });

    // engine.GetRenderContext().TransferBuffers(1, &transferInfo);
    // engine.FlushTransfers();

    Internal::GraphicsPipeline graphicsPipeline;
    Internal::GraphicsPiplineDesc pipelineDesc{};
    graphicsPipeline.renderPass = ctx.contextData.renderPass;
    InitGraphicsPipelineDesc(renderDevice, ctx, pipelineDesc);
    Internal::CreateGraphicsPipeline(renderDevice, graphicsPipeline, pipelineDesc);

    std::vector<TRE::Renderer::Buffer> uniformBuffers(1/*ctx.imagesCount*/);

    for (uint32 i = 0; i < 1/*ctx.imagesCount*/; i++) {
        uniformBuffers[i] = engine.GetRenderContext().CreateBuffer(sizeof(MVP), NULL, BufferUsage::UNIFORM_BUFFER,
            MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT);
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type            =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = (uint32_t)(1/*ctx.imagesCount*/);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount  = 1;
    poolInfo.pPoolSizes     = &poolSize;
    poolInfo.maxSets        = (uint32_t)(1/*ctx.imagesCount*/);

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(renderDevice.device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        ASSERTF(true, "failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(ctx.imagesCount, pipelineDesc.piplineLayoutDesc.descriptorSetLayout[0]);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool        = descriptorPool;
    allocInfo.descriptorSetCount    = static_cast<uint32_t>(1/*ctx.imagesCount*/);
    allocInfo.pSetLayouts           = layouts.data();

    std::vector<VkDescriptorSet> descriptorSets(1/*ctx.imagesCount*/);
    if (vkAllocateDescriptorSets(renderDevice.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (uint32 i = 0; i < 1/*ctx.imagesCount*/; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].GetAPIObject();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MVP);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType       = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet      = descriptorSets[i];
        descriptorWrite.dstBinding  = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo  = NULL; // Optional
        descriptorWrite.pTexelBufferView = NULL; // Optional

        vkUpdateDescriptorSets(renderDevice.device, 1, &descriptorWrite, 0, NULL);

        updateMVP(renderDevice, ctx, uniformBuffers[i]);
    }

    for (uint32 i = 0; i < 4; i++) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        //float r = sin(TRE::Math::ToRad((double)i));
        //float g = cos(TRE::Math::ToRad((double)i));
        //vertices[0].pos = TRE::vec3{ r, vertices[0].pos.y, 0 };
        //vertices[1].pos = TRE::vec3{ vertices[1].pos.x, g, 0 };

        vertices[i].color = TRE::vec3{ r, g, b };
    }


    INIT_BENCHMARK;

    time_t lasttime = time(NULL);

    while (window.isOpen()) {
        window.getEvent(ev);
       
        if (ev.Type == TRE::Event::TE_RESIZE) {
            Internal::UpdateSwapChain(ctx);
            continue;
        }

        engine.BeginFrame();

        /*if (time(NULL) != lasttime) {
            lasttime = time(NULL);
            srand(lasttime);

            for (uint32 i = 0; i < 4; i++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                // float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                vertices[i].color = TRE::vec3{ r, g, b };
            }

            memcpy(data, vertices.data(), vertexSize);
            engine.GetRenderContext().GetStagingManager().Stage(vertexIndexBuffer.GetAPIObject(), (void*)data, vertexSize);
        }*/

        /*TRE::Renderer::Internal::EditBuffer(renderDevice, staginVertexBuffer, vertexSize, vertices.data());
        engine.GetRenderContext().TransferBuffers(1, &transferInfo[0]);*/

        // updateMVP(renderDevice, ctx, uniformBuffers[0/*ctx.imagesCount*/]);
        RenderFrame(ctx.currentFrame, renderDevice, engine.GetRenderContext(), graphicsPipeline, vertexIndexBuffer, descriptorSets);

        engine.EndFrame();
        printFPS();
    }
    
    delete[] data;
    getchar();
}

#endif