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

#include "Shared.hpp"
#include "raytracing.hpp"


struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
};


#define CUBE


void updateCameraUBO(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::BufferHandle buffer, CameraUBO& ubo)
{
    const TRE::Renderer::Swapchain::SwapchainData& swapchainData = backend.GetRenderContext().GetSwapchain().GetSwapchainData();
    ubo.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    ubo.proj[1][1] *= -1;

    ubo.viewInverse = glm::inverse(ubo.view);
    ubo.projInverse = glm::inverse(ubo.proj);

    //ubo.viewInverse = glm::transpose(ubo.viewInverse);
    //ubo.projInverse = glm::transpose(ubo.projInverse);
    buffer->WriteToBuffer(sizeof(ubo), &ubo);
}

#define STOP_POINT(str) printf(str); printf("\n"); getchar();

int rt()
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
    //STOP_POINT("ENGINE INIT");
    

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
    Vertex vertecies[12 * 3];;
    vec3 verteciesData[12 * 3];

    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = TRE::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = TRE::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = TRE::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };

        verteciesData[i] = vertecies[i].pos;
    }

    std::vector<uint32> indicies(12 * 3);
    for (uint32 i = 0; i < 12 * 3; i++)
        indicies[i] = i;
#endif
    GraphicsState state;

    BufferHandle ubo = backend.CreateBuffer(BufferInfo::UniformBuffer(sizeof(CameraUBO)));

    ImageCreateInfo rtImageInfo = ImageCreateInfo::RtRenderTarget(SCR_WIDTH, SCR_HEIGHT);
    ImageHandle rtImage = backend.CreateImage(rtImageInfo);
    ImageViewCreateInfo rtViewInfo = ImageViewCreateInfo::ImageView(rtImage);
    ImageViewHandle rtView = backend.CreateImageView(rtViewInfo);

    // Out texture
    SamplerHandle outTexture = backend.CreateSampler(SamplerInfo::Sampler2D(rtImage));

    BufferHandle acclBuffer = backend.CreateBuffer(
        { sizeof(vertecies),
        BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER
        | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    BufferHandle acclIndexBuffer = backend.CreateBuffer(
        { indicies.size() * sizeof(uint32),
        BufferUsage::INDEX_BUFFER | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    backend.GetStagingManager().Stage(acclBuffer->GetApiObject(), &vertecies, sizeof(vertecies));
    backend.GetStagingManager().Stage(acclIndexBuffer->GetApiObject(), indicies.data(), indicies.size() * sizeof(uint32));
    backend.GetStagingManager().Flush();
    backend.GetStagingManager().WaitCurrent();

    //STOP_POINT("Staging Manager Flushing Buffer");

    /*VkDeviceAddress vertexData,
			VkDeviceSize vertexStride, uint32 vertexCount,
			VkDeviceAddress transformData = VK_NULL_HANDLE,
			VkDeviceAddress indexData = VK_NULL_HANDLE,
			const AsOffset& offset = {0, 0, 0, 0},
			uint32 flags = VK_GEOMETRY_OPAQUE_BIT_KHR,
			VkFormat vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
			VkIndexType indexType = VK_INDEX_TYPE_UINT32*/

    BlasCreateInfo info;
    info.AddGeometry(
        backend.GetRenderDevice().GetBufferAddress(acclBuffer),
        sizeof(Vertex), 12*3, NULL, 
        backend.GetRenderDevice().GetBufferAddress(acclIndexBuffer), 
        { 12, 0, 0, 0 }
    );

    BlasHandle blas = backend.CreateBlas(info, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
    //VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
    backend.RtBuildBlasBatchs();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtCompressBatch();
    printf("Object ID: %p\n", blas->GetApiObject());
    backend.RtSyncAcclBuilding();

    //STOP_POINT("Building Accl Structure");

    BlasInstance instance;
    instance.blas = blas;
    auto t = glm::mat4(1.f);
    memcpy(instance.transform, &t, sizeof(glm::mat4));
    TlasCreateInfo tlasInfo;
    tlasInfo.blasInstances.emplace_back(instance);
    TlasHandle tlas = backend.CreateTlas(tlasInfo);
    backend.BuildAS();
    backend.RtSyncAcclBuilding();

    ShaderProgram rtProgram(backend,
        {
            {"shaders/RT/rgen.spv", ShaderProgram::RGEN},
            {"shaders/RT/rmiss.spv", ShaderProgram::RMISS},
            {"shaders/RT/rchit.spv", ShaderProgram::RCHIT},
        });
    rtProgram.Compile();
    Pipeline rtPipeline(PipelineType::RAY_TRACE, &rtProgram);
    rtPipeline.Create(backend, 2);

    ShaderProgram postProgram(backend,
        {
            {"shaders/Post/post.vert.spv", ShaderProgram::VERTEX},
            {"shaders/Post/post.frag.spv", ShaderProgram::FRAGMENT},
        });
    postProgram.Compile();

    // STOP_POINT("Building Accl Structure (TLAS)");

    INIT_BENCHMARK;

    time_t lasttime = time(NULL);
    // TODO: shader specilization constants 
    CameraUBO uboData{};
    updateCameraUBO(backend, ubo, uboData);
    // D:/EngineDev/TrikytaEngine3D/Build/Renderer/x64/Debug/Renderer.exe
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
        //STOP_POINT("Begin Frame");

        auto cmd = backend.RequestCommandBuffer(CommandBuffer::Type::RAY_TRACING);
        cmd->BindShaderProgram(rtProgram);
        cmd->BindPipeline(rtPipeline);
        cmd->SetAccelerationStrucure(0, 0, *tlas);
        cmd->SetTexture(0, 1, *rtView);
        cmd->SetUniformBuffer(1, 0, *ubo);
        // cmd->PushConstants(VK_SHADER_STAGE_RAYGEN_BIT_KHR, &uboData, sizeof(uboData));
        cmd->TraceRays(SCR_WIDTH, SCR_HEIGHT);
        SemaphoreHandle semaphore;
        backend.Submit(cmd, NULL, 1, &semaphore);
        backend.AddWaitSemapore(CommandBuffer::Type::GENERIC, semaphore, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        /*FenceHandle fence;
        backend.Submit(cmd, &fence);
        fence->Wait();*/
        
        //STOP_POINT("Submitting RT CMD");

        auto cmd1 = backend.RequestCommandBuffer(CommandBuffer::Type::GENERIC);
        cmd1->BindShaderProgram(postProgram);
        //state.GetInputAssemblyState().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        //state.SaveChanges();
        cmd1->SetGraphicsState(state);
        cmd1->SetViewport({ 0, 0, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0, 1 });
        cmd1->SetScissor({ {0, 0}, {SCR_WIDTH, SCR_HEIGHT} });
        RenderPassInfo::Subpass subpass;
        cmd1->BeginRenderPass(GetRenderPass(backend, subpass));
        cmd1->SetTexture(0, 0, *rtView, *outTexture);
        //cmd1->SetTexture(0, 0, *textureView, *sampler);
        cmd1->Draw(3);
        cmd1->EndRenderPass();
        backend.Submit(cmd1);

        //STOP_POINT("Submitting Post render CMD");
        backend.EndFrame();
        printFPS();
    }

#if !defined(CUBE)
    delete[] data;
#endif
    getchar();
    return 0;
}