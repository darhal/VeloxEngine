#include "Renderer.hpp"
#include <Renderer/Core/Instance/Instance.hpp>
#include <Renderer/Core/Common/Globals.hpp>

Renderer::RenderContext* Renderer::Init()
{
    RenderContext* ctx = new RenderContext{0};

    if (CreateRenderContext(ctx) != 0) {
        delete ctx;
        return NULL;
    }

    return ctx;
}
