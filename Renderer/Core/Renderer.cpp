#include "Renderer.hpp"
#include <Renderer/Core/RenderInstance/RenderInstance.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/RenderContext/RenderContext.hpp>
#include <Renderer/Core/RenderDevice/RenderDevice.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>

TRE_NS_START

Renderer::RenderEngine::RenderEngine(TRE::Window* wnd)
{
    renderInstance.CreateRenderInstance();

    renderContext.CreateRenderContext(wnd, renderInstance.internal);
    renderDevice.CreateRenderDevice(renderInstance.internal, renderContext.internal);
    renderContext.InitRenderContext(renderInstance.internal, renderDevice.internal);
}

Renderer::RenderEngine::~RenderEngine()
{
    vkDeviceWaitIdle(renderDevice.internal.device);

    renderContext.DestroyRenderContext(renderInstance.internal, renderDevice.internal, renderContext.internal);
    renderDevice.DestroryRenderDevice();
    renderInstance.DestroyRenderInstance();
}

void Renderer::RenderEngine::BeginFrame()
{
    renderContext.BeginFrame(renderDevice.internal);
}

void Renderer::RenderEngine::EndFrame()
{
    renderContext.EndFrame(renderDevice.internal);
}

void Renderer::RenderEngine::FlushTransfers()
{
    renderContext.FlushTransfers(renderDevice.internal);
}

TRE_NS_END