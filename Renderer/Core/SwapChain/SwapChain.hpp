#pragma once

#include <Renderer/Common.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>
#include <Renderer/Core/Context/Context.hpp>

TRE_NS_START

namespace Renderer
{
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        TRE::Vector<VkSurfaceFormatKHR> formats;
        TRE::Vector<VkPresentModeKHR>   presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    void CreateSwapChain(SwapChain& swapChain, RenderContext& ctx);
    void DestroySwapChain(VkDevice device, SwapChain& swapChain);

    void Present(RenderContext& ctx, const TRE::Vector<VkCommandBuffer>& cmdbuff);

    void CreateImageViews(VkDevice device, SwapChain& swapChain);
    void CreateSyncObjects(VkDevice device, SwapChain& swapChain);
    void CreateCommandPool(RenderContext& ctx);
    void CreateCommandBuffers(RenderContext& ctx);

    void createGraphicsPipeline(RenderContext& ctx);
    void createRenderPass(RenderContext& ctx);
    void createFrameBuffers(RenderContext& ctx);

    VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent);
}

TRE_NS_END