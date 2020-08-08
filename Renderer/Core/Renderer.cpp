#include "Renderer.hpp"
#include <Renderer/Core/Instance/Instance.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/Context/Context.hpp>
#include <Renderer/Core/RenderDevice/RenderDevice.hpp>

TRE_NS_START

int32 Renderer::Init(RenderEngine& engine, TRE::Window* wnd)
{
    engine.renderInstance = new RenderInstance{0};
    engine.renderDevice   = new RenderDevice{0};
    engine.renderContext  = new RenderContext{0};

    if (CreateRenderInstance(engine.renderInstance) != 0) {
        return -1;
    }

    Renderer::CreateRenderContext(*engine.renderContext, wnd, *engine.renderInstance);
    Renderer::CreateRenderDevice(*engine.renderDevice, *engine.renderInstance, *engine.renderContext);
    Renderer::InitRenderContext(*engine.renderContext, *engine.renderInstance, *engine.renderDevice);
    return 0;
}

void Renderer::Destrory(RenderEngine& engine)
{
    vkDeviceWaitIdle(engine.renderDevice->device);

    Renderer::DestroyRenderContext(engine);
    Renderer::DestroryRenderDevice(*engine.renderDevice);
    Renderer::DestroyRenderInstance(*engine.renderInstance);
}

TRE_NS_END