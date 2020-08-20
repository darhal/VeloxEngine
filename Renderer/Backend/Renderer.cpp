#include "Renderer.hpp"
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/SwapChain/SwapChain.hpp>

TRE_NS_START

Renderer::RenderEngine::RenderEngine(TRE::Window* wnd)
{
    renderDevice.internal.renderContext = &renderContext.internal;
    renderContext.internal.renderDevice = &renderDevice.internal;

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