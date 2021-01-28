#include "pch.hpp"


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
#include "cube.hpp"

#include "Shared.hpp"
#include "raster.hpp"

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

#define CUBE

void updateMVP(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::RingBufferHandle buffer, const glm::vec3& pos)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    const TRE::Renderer::Swapchain::SwapchainData& swapchainData = backend.GetRenderContext().GetSwapchain().GetSwapchainData();

    MVP mvp{};

#if defined(CUBE)
    mvp.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
#endif
    mvp.model = glm::translate(mvp.model, pos);
    mvp.model = glm::rotate(mvp.model, time * TRE::Math::ToRad(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    mvp.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
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
    const TRE::Renderer::BufferHandle lightBuffer)
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


int raster()
{
    const unsigned int SCR_WIDTH = 640; //1920 / 2;
    const unsigned int SCR_HEIGHT = 480; //1080 / 2;

    using namespace TRE::Renderer;
    using namespace TRE;

    Event ev;
    Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderBackend backend{ &window };
    backend.InitInstance(RenderBackend::RAY_TRACING);
    // backend.SetSamplerCount(2);

    if (backend.GetMSAASamplerCount() == 1) {
        TRE_LOGI("Anti-Aliasing: Disabled");
    } else {
        TRE_LOGI("Anti-Aliasing: MSAA x%d", backend.GetMSAASamplerCount());
    }

    TRE_LOGI("Engine is up and running ...");
    Vertex vertecies[12 * 3];

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
        vertecies[i].color = TRE::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = TRE::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    BufferHandle vertexIndexBuffer = backend.CreateBuffer({ sizeof(vertecies), BufferUsage::VERTEX_BUFFER }, vertecies);
    BufferHandle cpuVertexBuffer = backend.CreateBuffer({ sizeof(vertecies), BufferUsage::TRANSFER_SRC, MemoryUsage::CPU_ONLY }, vertecies);

    glm::vec4 lightInfo[] = { glm::vec4(1.f, 0.5f, 0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.f) };
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
    return 0;
}