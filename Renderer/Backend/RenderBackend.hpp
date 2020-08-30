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

		Internal::RenderContext& GetCtxInternal() { return renderContext.internal; }
		Internal::RenderDevice& GetDevInternal() { return renderDevice.internal; }
		Internal::RenderInstance& GetInstInternal() { return renderInstance.internal; }

		Buffer CreateBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);

		Buffer CreateStagingBuffer(DeviceSize size, const void* data);

		RingBuffer CreateRingBuffer(DeviceSize size, const void* data, uint32 usage, MemoryUsage memoryUsage, uint32 queueFamilies = QueueFamilyFlag::NONE);
	
		FORCEINLINE MemoryAllocator& GetContextAllocator();

		FORCEINLINE StagingManager& GetStagingManager() { return stagingManager; }
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;

		MemoryAllocator	gpuMemoryAllocator;
		StagingManager	stagingManager;
	};
};

Renderer::MemoryAllocator& Renderer::RenderBackend::GetContextAllocator()
{
	return gpuMemoryAllocator;
}


TRE_NS_END