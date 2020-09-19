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

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderBackend
	{
	public:
		struct PerFrame
		{
			CommandPool commandPools[MAX_THREADS][(uint32)QueueTypes::MAX];
			StackAlloc<VkCommandBuffer, 32> submissions[(uint32)QueueTypes::MAX];
		};

		struct HandlePool
		{
			ObjectPool<CommandBuffer> commandBuffers;
			ObjectPool<Buffer>		  buffers;
			ObjectPool<RingBuffer>	  ringBuffers;
			ObjectPool<Image>		  images;
			ObjectPool<ImageView>	  imageViews;
			ObjectPool<Sampler>		  samplers;
		};

	public:
		RenderBackend(TRE::Window* wnd);

		~RenderBackend();

		void BeginFrame();

		void EndFrame();

		FORCEINLINE RenderInstance& GetRenderInstance() { return renderInstance; }
		FORCEINLINE RenderContext& GetRenderContext() { return renderContext; }
		FORCEINLINE RenderDevice& GetRenderDevice() { return renderDevice; }

		FORCEINLINE const RenderInstance& GetRenderInstance() const { return renderInstance; }
		FORCEINLINE const RenderContext& GetRenderContext() const { return renderContext; }
		FORCEINLINE const RenderDevice& GetRenderDevice() const { return renderDevice; }

		ImageHandle CreateImage(const ImageCreateInfo& createInfo, const void* data = NULL);

		ImageViewHandle CreateImageView(const ImageViewCreateInfo& createInfo);

		bool CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo);

		BufferHandle CreateBuffer(const BufferInfo& createInfo, const void* data = NULL);

		RingBufferHandle CreateRingBuffer(const BufferInfo& createInfo, const void* data = NULL);

		SamplerHandle CreateSampler(const SamplerInfo& createInfo);
	
		CommandBufferHandle RequestCommandBuffer(QueueTypes type);

		void Submit(CommandBufferHandle cmd);

		void CreateShaderProgram(const std::initializer_list<ShaderProgram::ShaderStage>& shaderStages, ShaderProgram* shaderProgramOut);

		DescriptorSetAllocator* RequestDescriptorSetAllocator(const DescriptorSetLayout& layout);

		// Render pass and framebuffer functionalities:
		const Framebuffer& RequestFramebuffer(const RenderPassInfo& info);

		const RenderPass& RequestRenderPass(const RenderPassInfo& info, bool compatible = true);

		FORCEINLINE MemoryAllocator& GetContextAllocator() { return gpuMemoryAllocator; }

		FORCEINLINE StagingManager& GetStagingManager() { return stagingManager; }

		FORCEINLINE ObjectPool<CommandBuffer>& GetCommandBufferPool() { return objectsPool.commandBuffers; }
	private:
		void Init();

		FORCEINLINE const PerFrame& Frame() const { return perFrame[renderContext.GetCurrentFrame()]; }

		FORCEINLINE PerFrame& Frame() { return perFrame[renderContext.GetCurrentFrame()]; }

		void ClearFrame();
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;

		MemoryAllocator	gpuMemoryAllocator;
		StagingManager	stagingManager;

		std::unordered_map<Hash, DescriptorSetAllocator> descriptorSetAllocators;
		std::unordered_map<Hash, RenderPass>			 renderPasses;
		FramebufferAllocator							 framebufferAllocator;

		PerFrame		perFrame[MAX_FRAMES];
		HandlePool		objectsPool;

		friend class Image;
	};
};

TRE_NS_END