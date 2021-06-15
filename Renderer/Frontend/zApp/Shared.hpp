#pragma once
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Backend/Backend.hpp>
#include "Camera.hpp"

using namespace TRE;

const unsigned int SCR_WIDTH = 800; //1920 / 2;
const unsigned int SCR_HEIGHT = 600; //1080 / 2;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex;
    glm::vec3 normal;

    Vertex(const glm::vec3& pos, const glm::vec3 color, glm::vec2 tex, glm::vec3 normal) :
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

static TRE::Renderer::RenderPassInfo GetRenderPass(TRE::Renderer::RenderDevice& dev, TRE::Renderer::RenderPassInfo::Subpass& subpass)
{
    using namespace TRE::Renderer;
    RenderPassInfo rpi = dev.GetSwapchainRenderPass(SwapchainRenderPass::DEPTH);

    /*if (backend.GetMSAASamplerCount() != 1) {
        rpi.colorAttachments[1] = &backend.GetTransientAttachment(
            dev.GetRenderContext()->GetSwapchain().GetExtent().width,
            dev.GetRenderContext()->GetSwapchain().GetExtent().height,
            dev.GetRenderContext()->GetSwapchain().GetFormat(),
            0, backend.GetMSAASamplerCount()1);

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
    }*/

    return rpi;
}


static void printFPS(float dt = 0.f)
{
    static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::steady_clock::now();
    static int fps;

    fps++;

    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
        oldTime = std::chrono::steady_clock::now();
        std::cout << "FPS: " << fps << " - dt: " << dt << "ms" << std::endl;
        fps = 0;
    }
}


static float lastX = (float)SCR_WIDTH / 2.0;
static float lastY = (float)SCR_HEIGHT / 2.0;
static bool firstMouse = true;
static bool start = false;
static int32 speed = 4;
static bool disableCamera = false;
static float deltaTime = 0.01f;

static bool HandleCameraEvent(Camera& camera, TRE::Event& e)
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