#pragma once

#include <Renderer/Common.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>
#include <Renderer/Core/Context/Context.hpp>

TRE_NS_START

namespace Renderer
{
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR            capabilities;
        TRE::Vector<VkSurfaceFormatKHR>     formats;
        TRE::Vector<VkPresentModeKHR>       presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    void CreateSwapChain(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice);
    void DestroySwapChain(const RenderDevice& renderDevice, RenderContext& ctx);
    void CleanupSwapChain(RenderContext& ctx, const RenderDevice& renderDevice);

    void UpdateSwapChain(RenderEngine& engine);

    void RecreateSwapChainInternal(RenderContext& ctx, const RenderInstance& renderInstance, const RenderDevice& renderDevice);

    void TransferMemory(RenderEngine& engine);

    void PrepareFrame(RenderEngine& engine);
    void Present(RenderEngine& engine);

    void CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx);
    void CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx);
    void CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx);

    void CreateFrameResources(const RenderDevice& renderDevice, RenderContext& ctx, const TRE::Vector<VkImage>& images);

    void CreateSwapChainRenderPass(const RenderDevice& renderDevice, RenderContext& ctx);

    void BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex);

    const ContextFrameResources& GetCurrentFrameResource(const RenderContext& ctx);

    ContextFrameResources& GetCurrentFrameResource(RenderContext& ctx);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent);
}

TRE_NS_END