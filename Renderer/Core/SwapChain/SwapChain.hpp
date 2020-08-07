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

    void CreateSwapChain(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice);
    void DestroySwapChain(const RenderDevice& renderDevice, RenderContext& ctx);

    void Present(RenderEngine& engine, const TRE::Vector<VkCommandBuffer>& cmdbuff);

    void CreateImageViews(const RenderDevice& renderDevice, RenderContext& ctx);
    void CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx);
    void CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx);
    void CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx);

    void createGraphicsPipeline(const RenderDevice& renderDevice, RenderContext& ctx);
    void createRenderPass(const RenderDevice& renderDevice, RenderContext& ctx);
    void createFrameBuffers(const RenderDevice& renderDevice, RenderContext& ctx);

    VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent);
}

TRE_NS_END