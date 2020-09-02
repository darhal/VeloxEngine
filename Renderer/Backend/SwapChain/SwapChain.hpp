#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Engine/Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;
    class RenderContext;

    class RENDERER_API Swapchain
    {
    public:
        struct SwapchainData
        {
            CONSTEXPR static uint32			MAX_FRAMES_IN_FLIGHT = MAX_FRAMES;
            CONSTEXPR static uint32			MAX_IMAGES_COUNT = 4;

            VkSemaphore						imageAcquiredSemaphores[MAX_FRAMES_IN_FLIGHT];
            VkSemaphore						drawCompleteSemaphores[MAX_FRAMES_IN_FLIGHT];
            VkFence							fences[MAX_FRAMES_IN_FLIGHT];

            // To use when using seprate presentation queue:
            VkSemaphore						imageOwnershipSemaphores[MAX_FRAMES_IN_FLIGHT];

            // To use when using seprate transfer queue:
            VkSemaphore						transferSemaphores[MAX_FRAMES_IN_FLIGHT];
            VkFence							transferSyncFence;

            // Swap chain images:
            VkImage							swapChainImages[MAX_IMAGES_COUNT];
            VkImageView						swapChainImageViews[MAX_IMAGES_COUNT];
            VkFramebuffer					swapChainFramebuffers[MAX_IMAGES_COUNT];

            // Other misc swapchain data:
            VkFormat						swapChainImageFormat;
            VkExtent2D						swapChainExtent;
        };

        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR            capabilities;
            TRE::Vector<VkSurfaceFormatKHR>     formats;
            TRE::Vector<VkPresentModeKHR>       presentModes;
        };
    public:
        Swapchain(const RenderDevice& renderDevice, RenderContext& renderContext);

        static SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        void CreateSwapchain();
        void CreateSwapchainResources();
        void DestroySwapchain();
        void CleanupSwapchain();

        void UpdateSwapchain();

        void RecreateSwapchain();

        void CreateSyncObjects();

        void CreateSwapchainRenderPass();

        /*void CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx);
        void CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx);
        void BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex);*/

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent);

    private:
        SwapchainData           swapChainData;
        SwapchainSupportDetails supportDetails;

        const RenderDevice&     renderDevice;
        RenderContext&          renderContext;
        VkSwapchainKHR          swapChain;
        VkRenderPass            renderPass;

        uint32					imagesCount;
        uint32					currentFrame;
        uint32					currentImage;

        bool					framebufferResized;
    };

    /*namespace Internal 
    {
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR            capabilities;
            TRE::Vector<VkSurfaceFormatKHR>     formats;
            TRE::Vector<VkPresentModeKHR>       presentModes;
        };

        SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        void CreateSwapchain(const RenderDevice& renderDevice, RenderContext& ctx);
        void CreateSwapchainResources(const RenderDevice& renderDevice, RenderContext& ctx);
        void DestroySwapchain(const RenderDevice& renderDevice, RenderContext& ctx);
        void CleanupSwapchain(RenderContext& ctx, const RenderDevice& renderDevice);

        void UpdateSwapchain(RenderContext& ctx);

        void RecreateSwapchainInternal(const RenderDevice& renderDevice, RenderContext& ctx);

        void CreateSyncObjects(const RenderDevice& renderDevice, RenderContext& ctx);
        void CreateCommandPool(const RenderDevice& renderDevice, RenderContext& ctx);
        void CreateCommandBuffers(const RenderDevice& renderDevice, RenderContext& ctx);

        void CreateSwapchainRenderPass(const RenderDevice& renderDevice, RenderContext& ctx);

        void BuildImageOwnershipCmd(const RenderDevice& renderDevice, RenderContext& ctx, uint32 imageIndex);

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const TRE::Vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const TRE::Vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& extent);
    }*/
}

TRE_NS_END