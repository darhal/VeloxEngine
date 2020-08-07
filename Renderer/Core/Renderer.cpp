#include "Renderer.hpp"
#include <Renderer/Core/Instance/Instance.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/Context/Context.hpp>
#include <Renderer/Core/RenderDevice/RenderDevice.hpp>

TRE_NS_START

int32 Renderer::Init(RenderEngine& engine, TRE::Window* wnd)
{
    engine.renderInstance = new RenderInstance();
    engine.renderDevice   = new RenderDevice();
    engine.renderContext  = new RenderContext();

    engine.renderContext->window = wnd;

    if (CreateRenderInstance(engine.renderInstance) != 0) {
        return -1;
    }

    Renderer::CreateRenderContext(*engine.renderContext, *engine.renderInstance);
    Renderer::CreateRenderDevice(*engine.renderDevice, *engine.renderInstance, *engine.renderContext);
    Renderer::InitRenderContext(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
    return 0;
}

TRE_NS_END