#pragma once
#include <vector>
#include <iostream>
#include <chrono>
#include <future>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Misc/stb_image.hpp>

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


const std::vector<uint16_t> indices = {
     0, 1, 2, 2, 3, 0

     // Cube:
     //0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

static TRE::Renderer::RenderPassInfo GetRenderPass(TRE::Renderer::RenderBackend& backend, TRE::Renderer::RenderPassInfo::Subpass& subpass)
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


static void printFPS(float dt = 0.f)
{
    static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
    static int fps;

    fps++;

    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
        oldTime = std::chrono::high_resolution_clock::now();
        std::cout << "FPS: " << fps << " - dt: " << dt << "ms" << std::endl;
        fps = 0;
    }
}
