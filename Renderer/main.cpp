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
#include <Renderer/Core/Renderer.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Buffer/Buffer.hpp>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Core/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Core/Common/Utils.hpp>
#include <Engine/Core/Misc/Maths/Maths.hpp>
#include <Engine/Core/Misc/Utils/Logging.hpp>

void InitRenderPassDesc(const TRE::Renderer::RenderEngine& engine, TRE::Renderer::RenderPassDesc& desc)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = engine.renderContext->swapChainData.swapChainImageFormat;
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

void InitGraphicsPipelineDesc(const TRE::Renderer::RenderEngine& engine, TRE::Renderer::GraphicsPiplineDesc& desc)
{
    auto vertShaderCode = TRE::Renderer::ReadShaderFile("shaders/vert.spv");
    auto fragShaderCode = TRE::Renderer::ReadShaderFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = TRE::Renderer::CreateShaderModule(engine.renderDevice->device, vertShaderCode);
    VkShaderModule fragShaderModule = TRE::Renderer::CreateShaderModule(engine.renderDevice->device, fragShaderCode);

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

    TRE::Renderer::VertexInputDesc vertexInputDesc;
    vertexInputDesc.binding = 0;
    vertexInputDesc.stride = sizeof(TRE::Renderer::Vertex);
    vertexInputDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputDesc.attribDesc.EmplaceBack(TRE::Renderer::VertextAttribDesc{ 0, offsetof(TRE::Renderer::Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT });
    vertexInputDesc.attribDesc.EmplaceBack(TRE::Renderer::VertextAttribDesc{ 1, offsetof(TRE::Renderer::Vertex, color), VK_FORMAT_R32G32B32_SFLOAT });
    desc.vertexInputDesc.EmplaceBack(vertexInputDesc);


    desc.inputAssemblyDesc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    desc.inputAssemblyDesc.primitiveRestartEnable = VK_FALSE;

    desc.rasterStateDesc.depthClampEnable = VK_FALSE;
    desc.rasterStateDesc.rasterizerDiscardEnable = VK_FALSE;
    desc.rasterStateDesc.polygonMode = VK_POLYGON_MODE_FILL;
    desc.rasterStateDesc.lineWidth = 1.0f;
    desc.rasterStateDesc.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
    desc.rasterStateDesc.frontFace = VK_FRONT_FACE_CLOCKWISE;
    desc.rasterStateDesc.depthBiasEnable = VK_FALSE;


    // desc.piplineLayoutDesc ;

    desc.viewport.x = 0.0f;
    desc.viewport.y = 0.0f;
    desc.viewport.width = (float)engine.renderContext->swapChainData.swapChainExtent.width;
    desc.viewport.height = (float)engine.renderContext->swapChainData.swapChainExtent.height;
    desc.viewport.minDepth = 0.0f;
    desc.viewport.maxDepth = 1.0f;

    desc.scissor.offset = { 0, 0 };
    desc.scissor.extent = engine.renderContext->swapChainData.swapChainExtent;

    desc.subpass = 0;
    desc.basePipelineHandle = 0;
    desc.basePipelineIndex = -1;
}

void RenderFrame(TRE::Renderer::RenderEngine& engine, TRE::Renderer::GraphicsPipeline& graphicsPipeline, TRE::Renderer::Buffer& vertexBuffer, TRE::Renderer::Buffer& indexBuffer)
{
    TRE::Renderer::RenderContext& ctx = *engine.renderContext;
    TRE::Renderer::RenderDevice& renderDevice = *engine.renderDevice;
    TRE::Renderer::SwapChainData& swapChainData = ctx.swapChainData;
    TRE::Renderer::RenderContextData& ctxData = ctx.contextData;
    TRE::Renderer::ContextFrameResources& ctxResource = TRE::Renderer::GetCurrentFrameResource(ctx);

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

    if (!renderDevice.isTransferQueueSeprate && ctxData.transferRequests) {
        VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

        vkCmdPipelineBarrier(currentCmdBuff,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT,
            1, &memoryBarrier, 0, NULL, 0, NULL
        );
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType                = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass           = graphicsPipeline.renderPass;
    renderPassInfo.framebuffer          = ctxResource.swapChainFramebuffer;
    renderPassInfo.renderArea.offset    = { 0, 0 };
    renderPassInfo.renderArea.extent    = swapChainData.swapChainExtent;
    renderPassInfo.clearValueCount      = 1;
    renderPassInfo.pClearValues         = &clearValue;

    vkCmdBeginRenderPass(currentCmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.pipeline);

    vkCmdSetViewport(currentCmdBuff, 0, 1, &viewport);
    vkCmdSetScissor(currentCmdBuff, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(currentCmdBuff, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(currentCmdBuff, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

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

    TRE::Event ev;
    TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    TRE::Renderer::RenderEngine engine{0};

    if (TRE::Renderer::Init(engine, &window) == 0) {
        puts("Rendering engine created with sucesss !");
    }

    TRE::Renderer::RenderContext& ctx = *engine.renderContext;
    TRE::Renderer::RenderDevice& renderDevice = *engine.renderDevice;
    TRE::Renderer::SwapChainData& swapChainData = engine.renderContext->swapChainData;

    uint32 queuFamilesIndiciesSeprate[] = {
        renderDevice.queueFamilyIndices.queueFamilies[TRE::Renderer::QFT_GRAPHICS],
        renderDevice.queueFamilyIndices.queueFamilies[TRE::Renderer::QFT_TRANSFER]
    };

    std::vector<TRE::Renderer::Vertex> vertices = {
        {TRE::vec3{-0.5f, -0.5f, 0.f},  TRE::vec3{1.0f, 0.0f, 0.0f}},
        {TRE::vec3{0.5f, -0.5f, 0.f},   TRE::vec3{0.0f, 1.0f, 0.0f}},
        {TRE::vec3{0.5f, 0.5f, 0.f},    TRE::vec3{0.0f, 0.0f, 1.0f}},
        {TRE::vec3{-0.5f, 0.5f, 0.f},   TRE::vec3{1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    size_t vertexSize = sizeof(vertices[0]) * vertices.size();

    uint32 queuesFamilyCount = 0;
    uint32* queuesFamilies = NULL;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (renderDevice.isTransferQueueSeprate){
        queuesFamilyCount = 2;
        queuesFamilies = queuFamilesIndiciesSeprate;
        sharingMode = VK_SHARING_MODE_CONCURRENT;
    }

    TRE::Renderer::Buffer staginVertexBuffer = TRE::Renderer::CreateStaginBuffer(renderDevice, vertexSize, vertices.data());
    TRE::Renderer::Buffer vertexBuffer =
        TRE::Renderer::CreateBuffer(renderDevice, sizeof(vertices[0]) * vertices.size(), NULL,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sharingMode,
            queuesFamilyCount, queuesFamilies
        );

    TRE::Renderer::Buffer staginIndexBuffer = TRE::Renderer::CreateStaginBuffer(renderDevice, sizeof(indices[0]) * indices.size(), indices.data());
    TRE::Renderer::Buffer indexBuffer =
        TRE::Renderer::CreateBuffer(renderDevice, sizeof(indices[0]) * indices.size(), NULL,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sharingMode,
            queuesFamilyCount, queuesFamilies
        );

    TRE::Renderer::TransferBufferInfo transferInfo[2];
    transferInfo[0].srcBuffer = &staginVertexBuffer;
    transferInfo[0].dstBuffer = &vertexBuffer;
    transferInfo[0].copyRegions.EmplaceBack(VkBufferCopy{0, 0, vertexSize});

    transferInfo[1].srcBuffer = &staginIndexBuffer;
    transferInfo[1].dstBuffer = &indexBuffer;
    transferInfo[1].copyRegions.EmplaceBack(VkBufferCopy{ 0, 0, sizeof(indices[0]) * indices.size() });

    TRE::Renderer::TransferBuffers(*engine.renderContext, 2, transferInfo);


    TRE::Renderer::GraphicsPipeline graphicsPipeline;
    TRE::Renderer::GraphicsPiplineDesc pipelineDesc{};
    graphicsPipeline.renderPass = ctx.contextData.renderPass;
    InitGraphicsPipelineDesc(engine, pipelineDesc);
    TRE::Renderer::CreateGraphicsPipeline(renderDevice, graphicsPipeline, pipelineDesc);

    uint32 i = 0;

    while (window.isOpen()) {
        window.getEvent(ev);
       
        if (ev.Type == TRE::Event::TE_RESIZE) {
            TRE::Renderer::UpdateSwapChain(engine);
            continue;
        }

        TRE::Renderer::PrepareFrame(engine);

        //if ((rand() % 11) <= 5) {
            //for (uint32 i = 0; i < 3; i++) {
                //float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                //float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                //float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                //float r = sin(TRE::Math::ToRad((double)i));
                //float g = cos(TRE::Math::ToRad((double)i));
                //vertices[0].pos = TRE::vec3{ r, vertices[0].pos.y, 0 };
                //vertices[1].pos = TRE::vec3{ vertices[1].pos.x, g, 0 };
            //}

            //TRE::Renderer::EditBuffer(renderDevice, staginVertexBuffer, vertexSize, vertices.data());
            //TRE::Renderer::TransferBuffers(*engine.renderContext, 1, &transferInfo);

            //srand(static_cast <unsigned> (time(0)));
        //}

        RenderFrame(engine, graphicsPipeline, vertexBuffer, indexBuffer);
     
        TRE::Renderer::Present(engine);

        i++;
        printFPS();
    }
    
    TRE::Renderer::Destrory(engine);
    getchar();
}

#endif