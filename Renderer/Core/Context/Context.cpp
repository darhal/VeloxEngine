#include "Context.hpp"
#include <Renderer/Window/Window.hpp>
#include <Renderer/Core/WindowSurface/WindowSurface.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Instance/Instance.hpp>
#include <unordered_set>

TRE_NS_START

void Renderer::CreateRenderContext(RenderContext& ctx, const RenderInstance& instance)
{
    ASSERT(instance.instance == VK_NULL_HANDLE);
    CreateWindowSurface(instance, ctx);



}

void Renderer::InitRenderContext(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice)
{
    ctx.swapChainData.swapChainExtent = VkExtent2D{ ctx.window->getSize().x, ctx.window->getSize().y };
    CreateSwapChain(ctx, renderInstance, renderDevice);
}

void Renderer::DestroyContext(RenderContext& p_ctx)
{
    //DestroySwapChain(p_ctx.device, p_ctx.swapChain);
    //vkDestroyDevice(p_ctx.device, NULL);
    //DestroryWindowSurface(p_ctx.instance->instance, p_ctx.surface);

    //if (p_ctx.instance) {
    //    DestroyRenderInstance(*p_ctx.instance);
    //}
}



TRE_NS_END