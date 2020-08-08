#include "Context.hpp"
#include <Renderer/Window/Window.hpp>
#include <Renderer/Core/WindowSurface/WindowSurface.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Instance/Instance.hpp>
#include <unordered_set>

TRE_NS_START

void Renderer::CreateRenderContext(RenderContext& ctx, TRE::Window* wnd, const RenderInstance& instance)
{
    ASSERT(instance.instance == VK_NULL_HANDLE);
    ctx.window = wnd;
    ctx.swapChainData.swapChainExtent = VkExtent2D{ ctx.window->getSize().x, ctx.window->getSize().y };

    CreateWindowSurface(instance, ctx);
}

void Renderer::InitRenderContext(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice)
{
    CreateSwapChain(ctx, renderInstance, renderDevice);
}

void Renderer::DestroyRenderContext(const RenderEngine& engine)
{
    DestroySwapChain(*engine.renderDevice, *engine.renderContext);
    DestroryWindowSurface(engine.renderInstance->instance, engine.renderContext->surface);

    // TODO: free window as well maybe ?? (probably not its stack allocated !)
}



TRE_NS_END