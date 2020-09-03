#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/MemoryAllocator/MemoryAllocator.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Buffers/RingBuffer.hpp>
#include <Renderer/Backend/StagingManager/StagingManager.hpp>
#include <Renderer/Core/Allocators/StackAllocator.hpp>
#include <Renderer/Backend/CommandList/CommandPool.hpp>
#include <Renderer/Backend/CommandList/CommandList.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderBackend
	{
	public:
		RenderBackend(TRE::Window* wnd);

		~RenderBackend();

		void BeginFrame();

		void EndFrame();

		FORCEINLINE RenderInstance& GetRenderInstance() { return renderInstance; }
		FORCEINLINE RenderContext& GetRenderContext() { return renderContext; }
		FORCEINLINE RenderDevice& GerRenderDevice() { return renderDevice; }
		FORCEINLINE const RenderInstance& GetRenderInstance() const { return renderInstance; }
		FORCEINLINE const RenderContext& GetRenderContext() const { return renderContext; }
		FORCEINLINE const RenderDevice& GerRenderDevice() const { return renderDevice; }

		Internal::RenderContext& GetCtxInternal() { return renderContext.internal; }
		Internal::RenderDevice& GetDevInternal() { return renderDevice.internal; }
		Internal::RenderInstance& GetInstInternal() { return renderInstance.internal; }

		Buffer CreateBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);

		Buffer CreateStagingBuffer(DeviceSize size, const void* data);

		RingBuffer CreateRingBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);
	
		CommandBufferHandle RequestCommandBuffer(QueueTypes type);

		void Submit(VkCommandBuffer cmd);

		FORCEINLINE MemoryAllocator& GetContextAllocator() { return gpuMemoryAllocator; }

		FORCEINLINE StagingManager& GetStagingManager() { return stagingManager; }
	private:
		void Init();
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;

		MemoryAllocator	gpuMemoryAllocator;
		StagingManager	stagingManager;

		struct PerFrame
		{
			CommandPool commandPools[MAX_THREADS][(uint32)QueueTypes::MAX];
			StackAlloc<VkCommandBuffer, 32> submissions[(uint32)QueueTypes::MAX];
		} perFrame[MAX_FRAMES];

		struct HandlePool
		{
			ObjectPool<CommandBuffer> commandBuffers;
		} objectsPool;

	private:
		FORCEINLINE const PerFrame& Frame() const { return perFrame[renderContext.GetCurrentFrame()]; }
		FORCEINLINE PerFrame& Frame() { return perFrame[renderContext.GetCurrentFrame()]; }

		void ClearFrame();
	};
};

TRE_NS_END