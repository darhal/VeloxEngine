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
#include <Renderer/Core/Alignement/Alignement.hpp>

#include "Shared.hpp"
#include "Camera.hpp"
#include "raytracing.hpp"

#define CUBE
#define STOP_POINT(str) printf(str); printf("\n"); getchar();

const unsigned int SCR_WIDTH = 640; //1920 / 2;
const unsigned int SCR_HEIGHT = 480; //1080 / 2;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool start = false;
int32 speed = 4;
bool disableCamera = false;

float deltaTime = 0.01f;

struct CameraUBO
{
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
};

bool operator!=(const VkExtent2D& e1, const VkExtent2D& e2)
{
    return e1.width != e2.width || e2.height != e2.height;
}

using namespace TRE::Renderer;
using namespace TRE;

bool HandleCameraEvent(Camera& camera, TRE::Event& e);

void updateCameraUBO(const TRE::Renderer::RenderBackend& backend, TRE::Renderer::BufferHandle buffer, Camera& cam)
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

int rt()
{
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
    
    Vertex vertecies[12 * 3];;

    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = TRE::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = TRE::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = TRE::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = TRE::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    std::vector<uint32> indicies(12 * 3);
    for (uint32 i = 0; i < 12 * 3; i++)
        indicies[i] = i;

    GraphicsState state;

    Camera camera;
    camera.SetPrespective(60.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 512.0f, true);

    BufferHandle ubo = backend.CreateBuffer(BufferInfo::UniformBuffer(sizeof(CameraUBO)));
    updateCameraUBO(backend, ubo, camera);

    auto rtImage = backend.CreateImage(ImageCreateInfo::RtRenderTarget(SCR_WIDTH, SCR_HEIGHT));
    auto rtView = backend.CreateImageView(ImageViewCreateInfo::ImageView(rtImage));

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

    BlasCreateInfo info;
    info.AddGeometry(
        backend.GetRenderDevice().GetBufferAddress(acclBuffer),
        sizeof(Vertex), 12*3, NULL, 
        backend.GetRenderDevice().GetBufferAddress(acclIndexBuffer), 
        { 12, 0, 0, 0 }
    );

    BlasHandle blas = backend.CreateBlas(info, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
    backend.RtBuildBlasBatchs();
    backend.RtCompressBatch();
    backend.RtSyncAcclBuilding();

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

    RenderContext& ctx = backend.GetRenderContext();
    auto lastExtent = ctx.GetSwapchainExtent();

    while (window.isOpen()) {
        auto tStart = std::chrono::high_resolution_clock::now();
        window.getEvent(ev);
        auto currExtent = ctx.GetSwapchainExtent();

        if (HandleCameraEvent(camera, ev)) {
            updateCameraUBO(backend, ubo, camera);
        }

        if (ev.Type == TRE::Event::TE_RESIZE || currExtent != lastExtent) {
            backend.GetRenderContext().GetSwapchain().UpdateSwapchain();

            rtImage = backend.CreateImage(ImageCreateInfo::RtRenderTarget(currExtent.width, currExtent.height));
            rtView = backend.CreateImageView(ImageViewCreateInfo::ImageView(rtImage));

            backend.GetStagingManager().Flush();

            camera.SetPrespective(60.0f, (float)currExtent.width / (float)currExtent.height, 0.1f, 512.0f, true);
            updateCameraUBO(backend, ubo, camera);
            lastExtent = currExtent;

            backend.GetStagingManager().WaitCurrent();
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
        
        auto cmd = backend.RequestCommandBuffer(CommandBuffer::Type::RAY_TRACING);

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
        
        backend.Submit(cmd);
        backend.EndFrame();

        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        deltaTime = (float)tDiff / 1000.0f; // seconds
        printFPS(tDiff);
    }

#if !defined(CUBE)
    delete[] data;
#endif
    getchar();
    return 0;
}


bool HandleCameraEvent(Camera& camera, TRE::Event& e)
{
    bool updated = false;

    if (disableCamera)
        return updated;

    if (e.Type == Event::TE_KEY_DOWN) {
        switch (e.Key.Code) {
        case Key::Up:
            speed += 1;
            printf("Changing camera speed to %d\n", speed);
            break;
        case Key::Down:
            speed -= 1;
            printf("Changing camera speed to %d\n", speed);
            break;
        case Key::Space:
            start = !start;
            deltaTime = 0;
            break;
        case Key::Z:
            camera.ProcessKeyboard(FORWARD, speed * deltaTime);
            updated = true;
            break;
        case Key::S:
            camera.ProcessKeyboard(BACKWARD, speed * deltaTime);
            updated = true;
            break;
        case Key::Q:
            camera.ProcessKeyboard(LEFT, speed * deltaTime);
            updated = true;
            break;
        case Key::D:
            camera.ProcessKeyboard(RIGHT, speed * deltaTime);
            updated = true;
            break;
        case Key::Escape:
            disableCamera = !disableCamera;
        default:
            break;
        }
    } else if (e.Type == Event::TE_MOUSE_MOVE) {
        float xpos = (float)e.Mouse.X;
        float ypos = (float)e.Mouse.Y;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);

        updated = true;
    }

    return updated;
}
