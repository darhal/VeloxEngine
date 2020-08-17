#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderContext
	{
	public:
		RenderContext();

		void CreateRenderContext(TRE::Window* wnd, const Internal::RenderInstance& instance);

		void InitRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice);

		void DestroyRenderContext(const Internal::RenderInstance& renderInstance, const Internal::RenderDevice& renderDevice, Internal::RenderContext& renderContext);

		void TransferBuffers(uint32 count, Internal::TransferBufferInfo* transferBufferInfo);

		Internal::ContextFrameResources& GetCurrentFrameResource();

		const Internal::ContextFrameResources& GetCurrentFrameResource() const;

		const Internal::SwapChainData& GetSwapChainData() const;

		FORCEINLINE uint32 GetImagesCount() const { return internal.imagesCount; }

		FORCEINLINE uint32 GetCurrentImageIndex() const { return internal.currentImage; }

		FORCEINLINE uint32 GetCurrentFrame() const { return internal.currentFrame; }

	private:
		void BeginFrame(Internal::RenderDevice& renderDevice);

		void EndFrame(Internal::RenderDevice& renderDevice);

		void ExecuteTransferMemory(VkQueue queue, VkCommandBuffer cmdBuff, VkPipelineStageFlags waitStage, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE);
	
		void FlushTransfers(Internal::RenderDevice& renderDevice);
	private:
		Internal::RenderContext	internal;

		friend class RenderEngine;
	};
}

TRE_NS_END