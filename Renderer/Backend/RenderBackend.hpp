#pragma once

#include <unordered_map>

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
#include <Renderer/Backend/Descriptors/DescriptorSetAlloc.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderProgram.hpp>
#include <Renderer/Backend/Images/Sampler.hpp>
#include <Renderer/Backend/RenderPass/Framebuffer.hpp>
#include <Renderer/Backend/RenderPass/FramebufferAllocator.hpp>
#include <Renderer/Backend/RenderPass/RenderPass.hpp>
#include <Renderer/Backend/Images/ImageHelper.hpp>
#include <Renderer/Backend/RenderPass/AttachmentAllocator.hpp>
#include <Renderer/Backend/Synchronization/Managers/FenceManager.hpp>
#include <Renderer/Backend/Synchronization/Fence/Fence.hpp>
#include <Renderer/Backend/Synchronization/Managers/SemaphoreManager.hpp>
#include <Renderer/Backend/Synchronization/Semaphore/Semaphore.hpp>
#include <Renderer/Backend/Synchronization/Managers/EventManager.hpp>
#include <Renderer/Backend/Synchronization/Event/Event.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/Pipeline/PipelineAllocator/PipelineAllocator.hpp>
#include <Renderer/Backend/RayTracing/BLAS/BLAS.hpp>
#include <Renderer/Backend/RayTracing/TLAS/TLAS.hpp>
#include <Renderer/Backend/RayTracing/ASBuilder.hpp>

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderBackend
	{
	public:
		RenderBackend(TRE::Window* wnd);

		~RenderBackend();

		void Shutdown();

		void InitInstance(uint32 usage = 0);

        // Frame managment:
		void BeginFrame();

		void EndFrame();


		// ..
		void SetSamplerCount(uint32 msaaSamplerCount = 1);

        // Getters:
        FORCEINLINE RenderInstance& GetRenderInstance() { return renderInstance; }
        FORCEINLINE RenderContext& GetRenderContext() { return renderContext; }
        FORCEINLINE RenderDevice& GetRenderDevice() { return renderDevice; }

        FORCEINLINE const RenderInstance& GetRenderInstance() const { return renderInstance; }
        FORCEINLINE const RenderContext& GetRenderContext() const { return renderContext; }
        FORCEINLINE const RenderDevice& GetRenderDevice() const { return renderDevice; }
	private:
		void ClearFrame();
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;
		TRE::Window*	window;

		uint32 msaaSamplerCount;
		uint32 enabledFeatures;
	};
};

TRE_NS_END
