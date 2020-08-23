#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/MemoryAllocator/MemoryAllocator.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Buffers/RingBuffer.hpp>
#include <Renderer/Backend/StagingManager/StagingManager.hpp>

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

		const Internal::ContextFrameResources& GetFrameResource(uint32 i) const;

		Internal::ContextFrameResources& GetFrameResource(uint32 i);

		Internal::ContextFrameResources& GetCurrentFrameResource();

		const Internal::ContextFrameResources& GetCurrentFrameResource() const;

		const Internal::SwapChainData& GetSwapChainData() const;

		Buffer CreateBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);

		Buffer CreateStagingBuffer(DeviceSize size, const void* data);

		RingBuffer CreateRingBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);

		FORCEINLINE uint32 GetImagesCount() const { return internal.imagesCount; }

		FORCEINLINE uint32 GetCurrentImageIndex() const { return internal.currentImage; }

		FORCEINLINE uint32 GetCurrentFrame() const { return internal.currentFrame; }

		FORCEINLINE MemoryAllocator& GetContextAllocator();

		FORCEINLINE StagingManager& GetStagingManager() { return stagingManager; }
	private:
		void BeginFrame(Internal::RenderDevice& renderDevice);

		void EndFrame(Internal::RenderDevice& renderDevice);

		void ExecuteTransferMemory(VkQueue queue, VkCommandBuffer cmdBuff, VkPipelineStageFlags waitStage, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE);
	
		void FlushTransfers(Internal::RenderDevice& renderDevice);
	private:
		Internal::RenderContext	internal;
		MemoryAllocator			gpuMemoryAllocator;
		StagingManager			stagingManager;

		friend class RenderBackend;
	};
}

Renderer::MemoryAllocator& Renderer::RenderContext::GetContextAllocator()
{
	return gpuMemoryAllocator;
}

TRE_NS_END