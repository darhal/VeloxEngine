#include "Renderer.hpp"
#include <Renderer/Core/Instance/Instance.hpp>
#include <Renderer/Core/Common/Globals.hpp>
#include <Renderer/Core/Context/Context.hpp>
#include <Renderer/Core/PhysicalDevice/PhysicalDevice.hpp>

TRE_NS_START

int32 Renderer::Init(Renderer::RenderContext& p_ctx)
{
    p_ctx.instance = new RenderInstance();

    if (CreateRenderInstance(p_ctx.instance) != 0) {
        return -1;
    }

    Renderer::InitContext(p_ctx);
    return 0;
}

TRE_NS_END