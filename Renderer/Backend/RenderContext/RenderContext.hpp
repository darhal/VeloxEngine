#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/StagingManager/StagingManager.hpp>
#include <Renderer/Backend/Swapchain/Swapchain.hpp>

TRE_NS_START

namespace Renderer
{
	class Swapchain;
	class RenderBackend;
	class RenderDevice;

	class RENDERER_API RenderContext
	{
	public:
		RenderContext(RenderBackend& backend);

		void CreateRenderContext(TRE::Window* wnd, const Internal::RenderInstance& instance);

		void InitRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice);

		void DestroyRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice, Internal::RenderContext& renderContext);

		FORCEINLINE VkFence GetFrameFence() const { return swapchain.swapchainData.fences[internal.currentFrame]; }

		FORCEINLINE VkSemaphore GetImageAcquiredSemaphore() const { return swapchain.swapchainData.imageAcquiredSemaphores[internal.currentFrame]; };

		FORCEINLINE VkSemaphore GetDrawCompletedSemaphore() const { return swapchain.swapchainData.drawCompleteSemaphores[internal.currentFrame]; };

		FORCEINLINE uint32 GetCurrentImageIndex() const { return internal.currentImage; }

		FORCEINLINE uint32 GetCurrentFrame() const { return internal.currentFrame; }

		FORCEINLINE uint32 GetPreviousFrame() const { return internal.previousFrame; }

		FORCEINLINE uint32 GetNumFrames() const { return internal.numFramesInFlight; }

		FORCEINLINE Window* GetWindow() const { return internal.window; }

		FORCEINLINE VkSurfaceKHR GetSurface() const { return internal.surface; }

		FORCEINLINE Swapchain& GetSwapchain() { return swapchain; }

		FORCEINLINE const Swapchain& GetSwapchain() const { return swapchain; }

		FORCEINLINE RenderDevice* GetRenderDevice() const { return renderDevice; }

		void BeginFrame(const RenderDevice& renderDevice, StagingManager& stagingManager);

		void EndFrame(const RenderDevice& renderDevice, const VkCommandBuffer* commandBuffers, const uint32 cmdBuffCount);
	private:
		Internal::RenderContext	internal;
		Swapchain swapchain;
		RenderDevice* renderDevice;

		friend class RenderBackend;
	};
}

TRE_NS_END