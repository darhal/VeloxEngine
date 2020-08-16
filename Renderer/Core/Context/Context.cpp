#include "Context.hpp"
#include <Renderer/Window/Window.hpp>
#include <Renderer/Core/WindowSurface/WindowSurface.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Core/Instance/Instance.hpp>
#include <unordered_set>

TRE_NS_START

Renderer::RenderContext::RenderContext() : internal{0}
{

}

void Renderer::RenderContext::CreateRenderContext(TRE::Window* wnd, const Internal::RenderInstance& instance)
{
    internal.window = wnd;
    internal.swapChainData.swapChainExtent = VkExtent2D{ internal.window->getSize().x, internal.window->getSize().y };

    CreateWindowSurface(instance, internal);
}

void Renderer::RenderContext::InitRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice)
{
    CreateSwapChain(renderDevice, internal);
}

void Renderer::RenderContext::DestroyRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice, Internal::RenderContext& renderContext)
{
    DestroySwapChain(renderDevice, renderContext);
    Internal::DestroryWindowSurface(renderInstance.instance, renderContext.surface);
}



TRE_NS_END