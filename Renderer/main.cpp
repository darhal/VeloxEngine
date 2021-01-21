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
#include <future>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Misc/Color/Color.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Core/Misc/Maths/Maths.hpp>
#include <Engine/Core/Misc/Utils/Logging.hpp>
#include "cube.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <Renderer/Misc/stb_image.hpp>

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 viewPos;
};

struct Vertex
{
    TRE::vec3 pos;
    TRE::vec3 color;
    TRE::vec2 tex;
    TRE::vec3 normal;

    Vertex(const TRE::vec3& pos, const TRE::vec3 color, TRE::vec2 tex, TRE::vec3 normal) :
        pos(pos), color(color), tex(tex), normal(normal)
    {
    }

    Vertex() = default;
};

/*std::vector<Vertex> vertices = {
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
};*/

const std::vector<uint16_t> indices = {
     0, 1, 2, 2, 3, 0

    // Cube:
    //0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

Vertex vertecies[12 * 3];

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

void updateMVP(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::RingBufferHandle buffer, const glm::vec3& pos = glm::vec3(-0.f, -0.f, 0.f))
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

    mvp.view    = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj    = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    mvp.proj[1][1] *= -1;
    
    mvp.viewPos = glm::vec3(1.0f, 1.0f, 0.7f);

    buffer->WriteToBuffer(sizeof(mvp), &mvp);
}

void RenderFrame(TRE::Renderer::RenderBackend& backend,
    const TRE::Renderer::ShaderProgram& program,
    TRE::Renderer::GraphicsState& state,
    const TRE::Renderer::BufferHandle vertexIndexBuffer,
    // VkDescriptorSet descriptorSet,
    const TRE::Renderer::RingBufferHandle uniformBuffer,
    const TRE::Renderer::ImageViewHandle texture,
    const TRE::Renderer::SamplerHandle sampler,
    const TRE::Renderer::BufferHandle lightBuffer
)
{
    using namespace TRE::Renderer;

    CommandBufferHandle cmd = backend.RequestCommandBuffer(CommandBuffer::Type::GENERIC);
    updateMVP(backend, uniformBuffer);

    cmd->BindShaderProgram(program);
    cmd->SetGraphicsState(state);

    cmd->SetDepthTest(true, true);
    cmd->SetDepthCompareOp(VK_COMPARE_OP_LESS);
    cmd->SetCullMode(VK_CULL_MODE_NONE);
    state.GetMultisampleState().rasterizationSamples = VkSampleCountFlagBits(backend.GetMSAASamplerCount());

    RenderPassInfo::Subpass subpass;
    cmd->BeginRenderPass(GetRenderPass(backend, subpass));

    cmd->BindVertexBuffer(*vertexIndexBuffer);
#if !defined(CUBE)
    cmd->BindIndexBuffer(*vertexIndexBuffer, sizeof(vertices[0]) * vertices.size());
#endif

    cmd->SetUniformBuffer(0, 0, *uniformBuffer);
    cmd->SetUniformBuffer(0, 2, *lightBuffer);
    cmd->SetTexture(0, 1, *texture, *sampler);

#if !defined(CUBE)
    cmd->DrawIndexed(indices.size());
#else
    cmd->Draw(36);
#endif
    
    cmd->EndRenderPass();
    backend.Submit(cmd);
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

uint32_t getMemoryType(const TRE::Renderer::RenderDevice& dev, uint32_t typeBits, const VkMemoryPropertyFlags& properties)
{
    for (uint32_t i = 0; i < dev.GetMemoryProperties().memoryTypeCount; i++) {
        if (((typeBits & (1 << i)) > 0) && (dev.GetMemoryProperties().memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    assert(0);
    return ~0u;
}

struct BufferDedicated
{
    VkBuffer       buffer{ VK_NULL_HANDLE };
    VkDeviceMemory allocation{ VK_NULL_HANDLE };
};

BufferDedicated createBuffer(const TRE::Renderer::RenderDevice& dev, const VkBufferCreateInfo& info_,
    const VkMemoryPropertyFlags memUsage_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
    BufferDedicated resultBuffer;
    // 1. Create Buffer (can be overloaded)
    
    vkCreateBuffer(dev.GetDevice(), &info_, nullptr, &resultBuffer.buffer);

    // 2. Find memory requirements
    VkMemoryRequirements2           memReqs{ VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    VkMemoryDedicatedRequirements   dedicatedRegs{ VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS };
    VkBufferMemoryRequirementsInfo2 bufferReqs{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 };

    bufferReqs.buffer = resultBuffer.buffer;
    memReqs.pNext = &dedicatedRegs;
    vkGetBufferMemoryRequirements2(dev.GetDevice(), &bufferReqs, &memReqs);

    // Device Address
    VkMemoryAllocateFlagsInfo memFlagInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO };
    if (info_.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        memFlagInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    // 3. Allocate memory
    VkMemoryAllocateInfo memAlloc{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memAlloc.allocationSize = memReqs.memoryRequirements.size;
    memAlloc.memoryTypeIndex = getMemoryType(dev, memReqs.memoryRequirements.memoryTypeBits, memUsage_);
    memAlloc.pNext = &memFlagInfo;
    vkAllocateMemory(dev.GetDevice(), &memAlloc, nullptr, &resultBuffer.allocation);

    // 4. Bind memory to buffer
    vkBindBufferMemory(dev.GetDevice(), resultBuffer.buffer, resultBuffer.allocation, 0);

    return resultBuffer;
}

BufferDedicated createBuffer(const TRE::Renderer::RenderDevice& dev,
    VkDeviceSize                size_ = 0,
    VkBufferUsageFlags          usage_ = VkBufferUsageFlags(),
    const VkMemoryPropertyFlags memUsage_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
    VkBufferCreateInfo info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    info.size = size_;
    info.usage = usage_;

    return createBuffer(dev, info, memUsage_);
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
    backend.InitInstance(RenderBackend::RAY_TRACING);
    backend.SetSamplerCount(2);

    if (backend.GetMSAASamplerCount() == 1) {
        TRE_LOGI("Anti-Aliasing: Disabled");
    } else {
        TRE_LOGI("Anti-Aliasing: MSAA x%d", backend.GetMSAASamplerCount());
    }

    TRE_LOGI("Engine is up and running ...");

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
    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = TRE::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = TRE::vec3{ 81.f/255.f, 254.f/255.f, 115.f/255.f };
        vertecies[i].normal = TRE::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    BufferHandle vertexIndexBuffer = backend.CreateBuffer({ sizeof(vertecies), BufferUsage::VERTEX_BUFFER }, vertecies);
    BufferHandle cpuVertexBuffer = backend.CreateBuffer({ sizeof(vertecies), BufferUsage::TRANSFER_SRC, MemoryUsage::CPU_ONLY }, vertecies);

    glm::vec4 lightInfo[] = {glm::vec4(1.f, 0.5f, 0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.f)};
    BufferHandle lightBuffer = backend.CreateBuffer({ sizeof(lightInfo), BufferUsage::STORAGE_BUFFER }, lightInfo);
#endif
    const uint32 checkerboard[] = {
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,

        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
    };

    // TODO: NEED WORK ON MEMORY FREEING!! (THIS IS DONE) (However we need to detect dedicated allocations from non dedicated allocs!)
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("assets/box1.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    ImageHandle texture = backend.CreateImage(ImageCreateInfo::Texture2D(texWidth, texHeight, true), pixels);
    ImageViewHandle textureView = backend.CreateImageView(ImageViewCreateInfo::ImageView(texture, VK_IMAGE_VIEW_TYPE_2D));
    SamplerHandle sampler = backend.CreateSampler(SamplerInfo::Sampler2D(texture));

    RingBufferHandle uniformBuffer = backend.CreateRingBuffer(BufferInfo::UniformBuffer(sizeof(MVP)), 3);

    GraphicsState state;
    // state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_LINE;
    /*auto& depthStencilState = state.GetDepthStencilState();
    depthStencilState.depthTestEnable = true;
    depthStencilState.depthWriteEnable = true;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    state.GetRasterizationState().cullMode = VK_CULL_MODE_NONE;
    state.GetMultisampleState().rasterizationSamples = VkSampleCountFlagBits(backend.GetMSAASamplerCount());
    state.SaveChanges();*/

    ShaderProgram program(backend,
        {
            {"shaders/vert.spv", ShaderProgram::VERTEX},
            {"shaders/frag.spv", ShaderProgram::FRAGMENT}
        });
    program.GetVertexInput().AddBinding(
        0, sizeof(Vertex),
        VertexInput::LOCATION_0 | VertexInput::LOCATION_1 | VertexInput::LOCATION_2 | VertexInput::LOCATION_3,
        { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, tex), offsetof(Vertex, normal) }
    );
    program.Compile();

    
    /*VkDeviceAddress vertexData,
        VkDeviceSize vertexStride, uint32 vertexCount,
        VkDeviceAddress transformData = VK_NULL_HANDLE,
        VkDeviceAddress indexData = VK_NULL_HANDLE,
        const AsOffset& offset = { 0, 0, 0, 0 },
        uint32 flags = VK_GEOMETRY_OPAQUE_BIT_KHR,
        VkFormat vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
        VkIndexType indexType = VK_INDEX_TYPE_UINT32*/

    //uint32 index[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    //BufferHandle indexBuffer = backend.CreateBuffer({ sizeof(index), BufferUsage::INDEX_BUFFER }, index);

    /*auto buff = createBuffer(backend.GetRenderDevice(), 128, BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER
        | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    VkBufferDeviceAddressInfo bufferAdrInfo;
    bufferAdrInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferAdrInfo.pNext = NULL;
    bufferAdrInfo.buffer = buff.buffer;
    auto adr = vkGetBufferDeviceAddressKHR(backend.GetRenderDevice().GetDevice(), &bufferAdrInfo);
    printf("Adr:%p\n", adr);*/

    BufferHandle acclBuffer = backend.CreateBuffer(
        { sizeof(vertecies), 
        BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER 
        | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY, 
        MemoryUsage::GPU_ONLY }
    );
    backend.GetStagingManager().Stage(acclBuffer->GetApiObject(), &vertecies, sizeof(Vertex) * 12 * 3);
    backend.GetStagingManager().Flush();
    backend.GetStagingManager().WaitCurrent();
    BlasCreateInfo info;
    info.AddGeometry(
        backend.GetRenderDevice().GetBufferAddress(acclBuffer),
        sizeof(Vertex), 12 * 3, NULL, NULL, {12, 0, 0, 0}
    );

    BlasHandle blas = backend.CreateBlas(info, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
        //VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
    backend.RtBuildBlasBatchs();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtCompressBatch();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtSyncAcclBuilding();
    {
        ShaderProgram rtProgram(backend,
            {
                {"shaders/RT/rgen.spv", ShaderProgram::RGEN},
                {"shaders/RT/rchit.spv", ShaderProgram::RCHIT},
                {"shaders/RT/rmiss.spv", ShaderProgram::RMISS}
            });
        rtProgram.Compile();
        Pipeline pipeline(PipelineType::RAY_TRACE, &rtProgram);
        pipeline.Create(backend.GetRenderDevice(), 2);

        // TODO: make an SBT
    }

    BlasInstance instance;
    instance.blas = blas;
    TlasCreateInfo tlasInfo;
    tlasInfo.blasInstances.emplace_back(instance);
    TlasHandle tlas = backend.CreateTlas(tlasInfo);
    backend.BuildAS();

    INIT_BENCHMARK;

    time_t lasttime = time(NULL);
    // TODO: shader specilization constants 

    while (window.isOpen()) {
        window.getEvent(ev);
       
        if (ev.Type == TRE::Event::TE_RESIZE) {
            backend.GetRenderContext().GetSwapchain().UpdateSwapchain();
            continue;
        } else if (ev.Type == TRE::Event::TE_KEY_UP) {
            if (ev.Key.Code == TRE::Key::L) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_LINE;
                state.SaveChanges();
            } else if (ev.Key.Code == TRE::Key::F) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_FILL;
                state.SaveChanges();
            }
        }

        backend.BeginFrame();
        RenderFrame(backend, program, state, vertexIndexBuffer, uniformBuffer, textureView, sampler, lightBuffer);

        /*for (int32_t i = 0; i < 12 * 3; i++) {
            float r = ((double)rand() / (RAND_MAX)) + 1;
            vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3] * r, g_vertex_buffer_data[i * 3 + 1] * r, g_vertex_buffer_data[i * 3 + 2] * r };
        }
        cpuVertexBuffer->WriteToBuffer(sizeof(vertecies), &vertecies);*/

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
//    bufferInfo.buffer = uniformBuffer.GetApiObject();
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