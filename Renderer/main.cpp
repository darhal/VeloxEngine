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
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Misc/Color/Color.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Core/Misc/Maths/Maths.hpp>
#include <Engine/Core/Misc/Utils/Logging.hpp>
// #include <Engine/Core/Misc/Utils/Image.hpp>
#include "cube.hpp"

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex
{
    TRE::vec3 pos;
    TRE::vec3 color;
    TRE::vec2 tex;

    Vertex(const TRE::vec3& pos, const TRE::vec3 color, TRE::vec2 tex) :
        pos(pos), color(color), tex(tex)
    {
    }

    Vertex() = default;
};

std::vector<Vertex> vertices = {
    { TRE::vec3{-0.5f, -0.5f, 0.f},  TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f} },
    { TRE::vec3{0.5f, -0.5f, 0.f},   TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f} },
    { TRE::vec3{0.5f, 0.5f, 0.f},    TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f} },
    { TRE::vec3{-0.5f, 0.5f, 0.f},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f} },

    // Cube:
    //{ TRE::vec3{-0.5, -0.5,  0.5},   TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f}  },
    //{ TRE::vec3{0.5, -0.5,  0.5},    TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f}  },
    //{ TRE::vec3{-0.5, 0.5,  0.5},  TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f}  },
    //{ TRE::vec3{0.5, 0.5,  0.5},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f}  },
    //{ TRE::vec3{-0.5, -0.5, -0.5},   TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f}  },
    //{ TRE::vec3{0.5, -0.5, -0.5},    TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f}  },
    //{ TRE::vec3{-0.5, 0.5, -0.5},  TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f}  },
    //{ TRE::vec3{0.5, 0.5, -0.5},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f}  },
};

const std::vector<uint16_t> indices = {
     0, 1, 2, 2, 3, 0

    // Cube:
    //0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

#define CUBE

TRE::Renderer::RenderPassInfo GetRenderPass(TRE::Renderer::RenderBackend& backend, TRE::Renderer::RenderPassInfo::Subpass& subpass)
{
    using namespace TRE::Renderer;
    RenderPassInfo rpi = backend.GetSwapchainRenderPass(SwapchainRenderPass::DEPTH);

    if (backend.GetMSAASamplerCount() != 1) {
        rpi.colorAttachments[1] = &backend.GetTransientAttachment(
            backend.GetRenderContext().GetSwapchain().GetExtent().width,
            backend.GetRenderContext().GetSwapchain().GetExtent().height,
            backend.GetRenderContext().GetSwapchain().GetFormat(),
            0, backend.GetMSAASamplerCount());

        rpi.clearAttachments = 1u << 1;
        rpi.storeAttachments = 1u << 1;
        rpi.clearColor[1] = { 0.051f, 0.051f, 0.051f, 0.0f };

        subpass.colorAttachmentsCount = 1;
        subpass.colorAttachments[0] = 1;
        subpass.resolveAttachmentsCount = 1;
        subpass.resolveAttachments[0] = 0;

        rpi.subpasses = &subpass;
        rpi.subpassesCount = 1;
        rpi.colorAttachmentCount = 2;
    }

    return rpi;
}

void updateMVP(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::RingBufferHandle buffer, const glm::vec3& pos = glm::vec3(0.f))
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    const TRE::Renderer::Swapchain::SwapchainData& swapchainData = backend.GetRenderContext().GetSwapchain().GetSwapchainData();

    MVP mvp{};

#if defined(CUBE)
    mvp.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
#endif
    mvp.model   = glm::translate(mvp.model, pos);
    mvp.model   = glm::rotate(mvp.model, time * TRE::Math::ToRad(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    mvp.view    = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj    = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    mvp.proj[1][1] *= -1;
    
    //mvp.model.transpose();
    //mvp.view.transpose();
    //mvp.proj.transpose();

    buffer->WriteToBuffer(sizeof(mvp), &mvp);
}

void RenderFrame(TRE::Renderer::RenderBackend& backend,
    TRE::Renderer::GraphicsPipeline& graphicsPipeline,
    const TRE::Renderer::BufferHandle vertexIndexBuffer,
    // VkDescriptorSet descriptorSet,
    const TRE::Renderer::RingBufferHandle uniformBuffer,
    const TRE::Renderer::ImageViewHandle texture,
    const TRE::Renderer::ImageViewHandle texture2,
    const TRE::Renderer::SamplerHandle sampler)
{
    using namespace TRE::Renderer;
    CommandBufferHandle currentCmdBuff = backend.RequestCommandBuffer(QueueTypes::GRAPHICS_ONLY);
    updateMVP(backend, uniformBuffer);

    currentCmdBuff->Begin();
    currentCmdBuff->BindPipeline(graphicsPipeline);


    RenderPassInfo::Subpass subpass;
    currentCmdBuff->BeginRenderPass(GetRenderPass(backend, subpass));

    currentCmdBuff->BindVertexBuffer(*vertexIndexBuffer);
#if !defined(CUBE)
    currentCmdBuff->BindIndexBuffer(*vertexIndexBuffer, sizeof(vertices[0]) * vertices.size());
#endif

    currentCmdBuff->SetUniformBuffer(0, 0, *uniformBuffer, uniformBuffer->GetCurrentOffset());
    currentCmdBuff->SetTexture(0, 1, *texture, *sampler);
#if !defined(CUBE)
    currentCmdBuff->DrawIndexed(indices.size());
#else
    currentCmdBuff->Draw(36);
#endif

    /*updateMVP(backend, uniformBuffer, {0.f, 1.5f, 0.f});
    currentCmdBuff->SetUniformBuffer(0, 0, *uniformBuffer, uniformBuffer->GetCurrentOffset());
    // currentCmdBuff->SetTexture(0, 1, *texture2, *sampler);
    currentCmdBuff->Draw(36);

    updateMVP(backend, uniformBuffer, { -0.f, -1.5f, 0.f });
    currentCmdBuff->SetUniformBuffer(0, 0, *uniformBuffer, uniformBuffer->GetCurrentOffset());
    currentCmdBuff->SetTexture(0, 1, *texture2, *sampler);
    currentCmdBuff->Draw(36);*/
    
    currentCmdBuff->EndRenderPass();
    currentCmdBuff->End();

    backend.Submit(currentCmdBuff);
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
    const unsigned int SCR_WIDTH = 640; //1920 / 2;
    const unsigned int SCR_HEIGHT = 480; //1080 / 2;

    using namespace TRE::Renderer;
    using namespace TRE;

    Event ev;
    Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderBackend backend{ &window };
    backend.SetSamplerCount(16);
    printf("MSAA: x%d\n", backend.GetMSAASamplerCount());

    uint32 queueFamilies = QueueFamilyFlag::NONE;

    if (backend.GetRenderDevice().IsTransferQueueSeprate()) {
        queueFamilies = QueueFamilyFlag::TRANSFER | QueueFamilyFlag::GRAPHICS;
    }

#if !defined(CUBE)
    size_t vertexSize = sizeof(vertices[0]) * vertices.size();
    size_t indexSize = sizeof(indices[0]) * indices.size();
    char* data = new char[vertexSize + indexSize];
    memcpy(data, vertices.data(), vertexSize);
    memcpy(data + vertexSize, indices.data(), indexSize);

    BufferHandle vertexIndexBuffer = backend.CreateBuffer(
        { vertexSize + indexSize, BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER | BufferUsage::INDEX_BUFFER }, 
        data);
#else
    Vertex vertecies[12 * 3];
    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = TRE::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
    }
    BufferHandle vertexIndexBuffer = backend.CreateBuffer(
        { sizeof(vertecies), BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER },
        vertecies);
#endif
    const uint32 checkerboard[] = {
        0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u,
        0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u,
    };

    const uint32 checkerboard2[] = {
        0u, 0u, 0u, 0u,
        ~0u, ~0u, ~0u, ~0u,
        0u, 0u, 0u, 0u,
        ~0u, ~0u, ~0u, ~0u,
    };

    ImageHandle texture = backend.CreateImage(ImageCreateInfo::Texture2D(4, 4), checkerboard);
    ImageViewHandle textureView = backend.CreateImageView(ImageViewCreateInfo::ImageView(texture, VK_IMAGE_VIEW_TYPE_2D));
    SamplerHandle sampler = backend.CreateSampler(SamplerInfo::Sampler2D());

    ImageHandle texture2 = backend.CreateImage(ImageCreateInfo::Texture2D(4, 4), checkerboard2);
    ImageViewHandle textureView2 = backend.CreateImageView(ImageViewCreateInfo::ImageView(texture2, VK_IMAGE_VIEW_TYPE_2D));

    RingBufferHandle uniformBuffer = backend.CreateRingBuffer(BufferInfo::UniformBuffer(sizeof(MVP)), 10);

    GraphicsPipeline graphicsPipeline;
    GraphicsState state;

    auto& depthStencilState = state.GetDepthStencilState();
    depthStencilState.depthTestEnable = true;
    depthStencilState.depthWriteEnable = true;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;

    state.GetRasterizationState().cullMode = VK_CULL_MODE_NONE;

    state.GetMultisampleState().rasterizationSamples = VkSampleCountFlagBits(backend.GetMSAASamplerCount());

    graphicsPipeline.GetShaderProgram().Create(backend,
        { 
            {"shaders/vert.spv", ShaderProgram::VERTEX_SHADER}, 
            {"shaders/frag.spv", ShaderProgram::FRAGMENT_SHADER} 
        }
    );

    graphicsPipeline.GetShaderProgram().GetVertexInput().AddBinding(
        0, sizeof(Vertex), 
        VertexInput::LOCATION_0 | VertexInput::LOCATION_1 | VertexInput::LOCATION_2, 
        { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, tex) }
    );


    RenderPassInfo::Subpass subpass;
    const RenderPass& rp = backend.RequestRenderPass(GetRenderPass(backend, subpass));
    graphicsPipeline.SetRenderPass(rp.GetAPIObject());
    // graphicsPipeline.SetRenderPass(backend.GetRenderContext().GetSwapchain().GetRenderPass());
    graphicsPipeline.Create(backend.GetRenderContext(), state);

    INIT_BENCHMARK;

    time_t lasttime = time(NULL);

    while (window.isOpen()) {
        window.getEvent(ev);
       
        if (ev.Type == TRE::Event::TE_RESIZE) {
            backend.GetRenderContext().GetSwapchain().UpdateSwapchain();
            continue;
        }

        backend.BeginFrame();
        RenderFrame(backend, graphicsPipeline, vertexIndexBuffer, uniformBuffer, textureView, textureView2, sampler);
        backend.EndFrame();

        printFPS();
    }
    
#if !defined(CUBE)
    delete[] data;
#endif
    getchar();
}

#endif



//DescriptorSetAllocator* alloc = backend.RequestDescriptorSetAllocator(graphicsPipeline.GetShaderProgram().GetPipelineLayout().GetDescriptorSetLayout(0));
//VkDescriptorSet descriptorSet = alloc->Allocate();

//for (uint32 i = 0; i < 1/*ctx.imagesCount*/; i++) {
//    VkDescriptorBufferInfo bufferInfo{};
//    bufferInfo.buffer = uniformBuffer.GetAPIObject();
//    bufferInfo.offset = 0;
//    bufferInfo.range  = sizeof(MVP);

//    VkWriteDescriptorSet descriptorWrite{};
//    descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//    descriptorWrite.dstSet           = descriptorSet;
//    descriptorWrite.dstBinding       = 0;
//    descriptorWrite.dstArrayElement  = 0;
//    descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    descriptorWrite.descriptorCount  = 1;
//    descriptorWrite.pBufferInfo      = &bufferInfo;
//    descriptorWrite.pImageInfo       = NULL; // Optional
//    descriptorWrite.pTexelBufferView = NULL; // Optional

//    vkUpdateDescriptorSets(renderDevice.device, 1, &descriptorWrite, 0, NULL);
//}




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
// vkCmdDraw(currentCmdBuff, 3, 1, 0, 0);

//VkImageSubresourceRange imgRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
//vkCmdClearColorImage(currentCmdBuff, swapChainData.swapChainImages[currentBuffer], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imgRange);