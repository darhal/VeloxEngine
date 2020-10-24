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

TRE_NS_START

namespace Renderer
{
	class RENDERER_API RenderBackend
	{
	public:
		struct PerFrame
		{
			CommandPool commandPools[MAX_THREADS][(uint32)CommandBuffer::Type::MAX];
			StaticVector<CommandBufferHandle> submissions[(uint32)CommandBuffer::Type::MAX];

			struct QueueData
			{
				StaticVector<SemaphoreHandle> waitSemaphores;
				StaticVector<VkPipelineStageFlags> waitStages;
			} queueData[(uint32)CommandBuffer::Type::MAX];

			StaticVector<VkFence>		  waitFences;

			TRE::Vector<VkPipeline>       destroyedPipelines;
			TRE::Vector<VkFramebuffer>    destroyedFramebuffers;
			TRE::Vector<VkImage>	      destroyedImages;
			TRE::Vector<VkImageView>      destroyedImageViews;
			TRE::Vector<VkBuffer>		  destroyedBuffers;
			TRE::Vector<VkBufferView>	  destroyedBufferViews;
			TRE::Vector<VkRenderPass>	  destroyedRenderPasses;
			TRE::Vector<VkDescriptorPool> destroyedDescriptorPool;
			TRE::Vector<VkSemaphore>	  destroyedSemaphores;
			TRE::Vector<VkEvent>		  destroyedEvents;

			TRE::Vector<VkFence>		  recycleFences;
			TRE::Vector<VkSemaphore>	  recycleSemaphores;

			TRE::Vector<VkDeviceMemory>	  freedMemory;

			bool shouldDestroy = false;
		};

		struct HandlePool
		{
			ObjectPool<CommandBuffer> commandBuffers;
			ObjectPool<Buffer>		  buffers;
			ObjectPool<RingBuffer>	  ringBuffers;
			ObjectPool<Image>		  images;
			ObjectPool<ImageView>	  imageViews;
			ObjectPool<Sampler>		  samplers;
			ObjectPool<Fence>		  fences;
			ObjectPool<Semaphore>	  semaphores;
			ObjectPool<PipelineEvent> events;
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

		void SetSamplerCount(uint32 msaaSamplerCount = 1);

		FenceHandle RequestFence();

		void ResetFence(VkFence fence, bool isWaited);

		SemaphoreHandle RequestSemaphore();

		PiplineEventHandle RequestPiplineEvent();

		ImageHandle CreateImage(const ImageCreateInfo& createInfo, const void* data = NULL);

		ImageViewHandle CreateImageView(const ImageViewCreateInfo& createInfo);

		bool CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo);

		BufferHandle CreateBuffer(const BufferInfo& createInfo, const void* data = NULL);

		RingBufferHandle CreateRingBuffer(const BufferInfo& createInfo, const uint32 ringSize = NUM_FRAMES, const void* data = NULL);

		SamplerHandle CreateSampler(const SamplerInfo& createInfo);
	
		CommandBufferHandle RequestCommandBuffer(CommandBuffer::Type type = CommandBuffer::Type::GENERIC);

		void Submit(CommandBufferHandle cmd, FenceHandle* fence  = NULL, uint32 semaphoreCount = 0, SemaphoreHandle* semaphores = NULL);

		void SubmitQueue(CommandBuffer::Type type, FenceHandle* fence = NULL, uint32 semaphoreCount = 0, SemaphoreHandle* semaphores = NULL);

		void AddWaitSemapore(CommandBuffer::Type type, SemaphoreHandle semaphore, VkPipelineStageFlags stages, bool flush = false);

		void FlushQueue(CommandBuffer::Type type);

		void FlushFrame();

		void CreateShaderProgram(const std::initializer_list<ShaderProgram::ShaderStage>& shaderStages, ShaderProgram* shaderProgramOut);

		DescriptorSetAllocator* RequestDescriptorSetAllocator(const DescriptorSetLayout& layout);

		// Render pass and framebuffer functionalities:
		const Framebuffer& RequestFramebuffer(const RenderPassInfo& info, const RenderPass* rp = NULL);

		const RenderPass& RequestRenderPass(const RenderPassInfo& info, bool compatible = false);

		RenderPassInfo GetSwapchainRenderPass(SwapchainRenderPass style);

		ImageView& GetTransientAttachment(uint32 width, uint32 height, VkFormat format, uint32 index = 0, uint32 samples = 1, uint32 layers = 1);

		void DestroyPendingObjects(PerFrame& frame);

		void DestroyImage(VkImage image);

		void DestroyImageView(VkImageView view);

		void DestroyFramebuffer(VkFramebuffer fb);

		void FreeMemory(VkDeviceMemory memory);

		void RecycleSemaphore(VkSemaphore sem);

		void DestroySemaphore(VkSemaphore sem);

		void DestroryEvent(VkEvent event);

		FORCEINLINE uint32 GetMSAASamplerCount() const { return msaaSamplerCount; }

		FORCEINLINE MemoryAllocator& GetContextAllocator() { return gpuMemoryAllocator; }

		FORCEINLINE StagingManager& GetStagingManager() { return stagingManager; }

		FORCEINLINE ObjectPool<CommandBuffer>& GetCommandBufferPool() { return objectsPool.commandBuffers; }

		FORCEINLINE HandlePool& GetObjectsPool() { return objectsPool; }
	private:
		FORCEINLINE const PerFrame& Frame() const { return perFrame[renderContext.GetCurrentFrame()]; }

		FORCEINLINE PerFrame& Frame() { return perFrame[renderContext.GetCurrentFrame()]; }

		FORCEINLINE const PerFrame& PreviousFrame() const { return perFrame[renderContext.GetPreviousFrame()]; }

		FORCEINLINE PerFrame& PreviousFrame() { return perFrame[renderContext.GetPreviousFrame()]; }

		CommandBuffer::Type GetPhysicalQueueType(CommandBuffer::Type type);

		PerFrame::QueueData& GetQueueData(CommandBuffer::Type type);

		StaticVector<CommandBufferHandle>& GetQueueSubmissions(CommandBuffer::Type type);

		VkQueue GetQueue(CommandBuffer::Type type);

		void Init();

		void ClearFrame();

		void SubmitEmpty(CommandBuffer::Type type, FenceHandle* fence, uint32 semaphoreCount, SemaphoreHandle* semaphores);
	private:
		RenderInstance	renderInstance;
		RenderDevice	renderDevice;
		RenderContext	renderContext;

		MemoryAllocator	 gpuMemoryAllocator;
		StagingManager	 stagingManager;
		FenceManager	 fenceManager;
		SemaphoreManager semaphoreManager;
		EventManager	 eventManager;

		std::unordered_map<Hash, DescriptorSetAllocator> descriptorSetAllocators;
		std::unordered_map<Hash, RenderPass>			 renderPasses;
		FramebufferAllocator							 framebufferAllocator;
		AttachmentAllocator								 transientAttachmentAllocator;
		
		PerFrame		perFrame[MAX_FRAMES];
		HandlePool		objectsPool;

		uint32 msaaSamplerCount;

		friend class Image;
		friend class Swapchain;
		friend class Semaphore;
		friend class Fence;
	};
};

TRE_NS_END