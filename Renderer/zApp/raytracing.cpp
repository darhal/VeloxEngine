
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include "Shared.hpp"
#include "Camera.hpp"
#include "raytracing.hpp"

#define CUBE
#define STOP_POINT(str) printf(str); printf("\n"); getchar();

struct CameraUBO
{
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
};

bool operator!=(const VkExtent2D& e1, const VkExtent2D& e2)
{
    return e1.width != e2.width || e1.height != e2.height;
}

using namespace TRE::Renderer;
using namespace TRE;

bool HandleCameraEvent(Camera& camera, TRE::Event& e);

void updateCameraUBO(const TRE::Renderer::RenderDevice& dev, TRE::Renderer::BufferHandle buffer, Camera& cam)
{
    /*const TRE::Renderer::Swapchain::SwapchainData& swapchainData = backend.GetRenderContext().GetSwapchain().GetSwapchainData();
    auto view = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto proj = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    proj[1][1] *= -1;

    ubo.viewInverse = glm::inverse(view);
    ubo.projInverse = glm::inverse(proj);*/

    //ubo.viewInverse = glm::transpose(ubo.viewInverse);
    //ubo.projInverse = glm::transpose(ubo.projInverse);
    CameraUBO ubo{};
    ubo.projInverse = glm::inverse(cam.GetPrespective());
    ubo.viewInverse = glm::inverse(cam.GetViewMatrix());
    buffer->WriteToBuffer(sizeof(ubo), &ubo);
}

int rt(RenderBackend& backend)
{
    using namespace TRE::Renderer;
    using namespace TRE;

    auto& window = *backend.GetRenderContext().GetWindow();
    Event ev;

    auto& dev = backend.GetRenderDevice();

    /*if (backend.GetMSAASamplerCount() == 1) {
        TRE_LOGI("Anti-Aliasing: Disabled");
    } else {
        TRE_LOGI("Anti-Aliasing: MSAA x%d", backend.GetMSAASamplerCount());
    }*/

    TRE_LOGI("Engine is up and running ...");
    
    Vertex vertecies[12 * 3];;

    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = glm::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = glm::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = glm::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = glm::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    std::vector<uint32> indicies(12 * 3);
    for (uint32 i = 0; i < 12 * 3; i++)
        indicies[i] = i;

    GraphicsState state;

    Camera camera;
    camera.SetPrespective(60.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 512.0f, true);

    BufferHandle ubo = dev.CreateBuffer(BufferInfo::UniformBuffer(sizeof(CameraUBO)));
    updateCameraUBO(dev, ubo, camera);

    auto rtImage = dev.CreateImage(ImageCreateInfo::RtRenderTarget(SCR_WIDTH, SCR_HEIGHT));
    auto rtView = dev.CreateImageView(ImageViewCreateInfo::ImageView(rtImage));

    BufferHandle acclBuffer = dev.CreateBuffer(
        { sizeof(vertecies),
        BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER
        | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    BufferHandle acclIndexBuffer = dev.CreateBuffer(
        { indicies.size() * sizeof(uint32),
        BufferUsage::INDEX_BUFFER | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCLS_BUILD_INPUT_READ_ONLY,
        MemoryUsage::GPU_ONLY }
    );
    dev.GetStagingManager().Stage(acclBuffer->GetApiObject(), &vertecies, sizeof(vertecies));
    dev.GetStagingManager().Stage(acclIndexBuffer->GetApiObject(), indicies.data(), indicies.size() * sizeof(uint32));
    dev.GetStagingManager().Flush();
    dev.GetStagingManager().WaitPrevious();

    BlasCreateInfo info;
    info.AddGeometry(
        backend.GetRenderDevice().GetBufferAddress(acclBuffer),
        sizeof(Vertex), 12*3, VK_NULL_HANDLE,
        backend.GetRenderDevice().GetBufferAddress(acclIndexBuffer), 
        { 12, 0, 0, 0 }
    );

    BlasHandle blas = dev.CreateBlas(info, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
    dev.RtBuildBlasBatchs();
    dev.RtCompressBatch();
    dev.RtSyncAcclBuilding();

    BlasInstance instance;
    instance.blas = blas;
    auto t = glm::mat4(1.f);
    memcpy(instance.transform, &t, sizeof(glm::mat4));
    TlasCreateInfo tlasInfo;
    tlasInfo.blasInstances.emplace_back(instance);
    TlasHandle tlas = dev.CreateTlas(tlasInfo);
    dev.BuildAS();
    dev.RtSyncAcclBuilding();

    ShaderProgram rtProgram(dev,
        {
            {"shaders/RT/rgen.spv", ShaderProgram::RGEN},
            {"shaders/RT/rmiss.spv", ShaderProgram::RMISS},
            {"shaders/RT/rchit.spv", ShaderProgram::RCHIT},
        });
    rtProgram.Compile();
    Pipeline rtPipeline(PipelineType::RAY_TRACE, &rtProgram);
    rtPipeline.Create(dev, 2);

    RenderContext& ctx = backend.GetRenderContext();
    auto lastExtent = ctx.GetSwapchainExtent();

    while (window.isOpen()) {
        auto tStart = std::chrono::high_resolution_clock::now();
        window.getEvent(ev);
        auto currExtent = ctx.GetSwapchainExtent();

        if (HandleCameraEvent(camera, ev)) {
            updateCameraUBO(dev, ubo, camera);
        }

        if (ev.Type == TRE::Event::TE_RESIZE || currExtent != lastExtent) {
            ctx.GetSwapchain().QueueSwapchainUpdate();

            rtImage = dev.CreateImage(ImageCreateInfo::RtRenderTarget(currExtent.width, currExtent.height));
            rtView = dev.CreateImageView(ImageViewCreateInfo::ImageView(rtImage));

            dev.GetStagingManager().Flush();

            camera.SetPrespective(60.0f, (float)currExtent.width / (float)currExtent.height, 0.1f, 512.0f, true);
            updateCameraUBO(dev, ubo, camera);
            lastExtent = currExtent;

            dev.GetStagingManager().WaitPrevious();
            printf("Image re-created!\n");
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
        
        auto cmd = dev.RequestCommandBuffer(CommandBuffer::Type::RAY_TRACING);

        cmd->BindShaderProgram(rtProgram);
        cmd->BindPipeline(rtPipeline);
        cmd->SetAccelerationStrucure(0, 0, *tlas);
        cmd->SetTexture(0, 1, *rtView);
        cmd->SetUniformBuffer(0, 2, *ubo);
        // cmd->PushConstants(VK_SHADER_STAGE_RAYGEN_BIT_KHR, &uboData, sizeof(uboData));
        cmd->TraceRays(currExtent.width, currExtent.height);

        cmd->ChangeImageLayout(*ctx.GetCurrentSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmd->ChangeImageLayout(*rtImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        cmd->CopyImage(*rtImage, *ctx.GetCurrentSwapchainImage(), { currExtent.width, currExtent.height, 1 } );
        cmd->ChangeImageLayout(*ctx.GetCurrentSwapchainImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        cmd->ChangeImageLayout(*rtImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        
        dev.Submit(cmd);
        backend.EndFrame();

        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        deltaTime = (float)tDiff / 1000.0f; // seconds
        printFPS((float)tDiff);
    }

#if !defined(CUBE)
    delete[] data;
#endif
    getchar();
    return 0;
}
