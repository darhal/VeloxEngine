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

			TRE::Vector<VkPipeline>       destroyedPipelines;
			TRE::Vector<VkFramebuffer>    destroyedFramebuffers;
			TRE::Vector<VkImage>	      destroyedImages;
			TRE::Vector<VkImageView>      destroyedImageViews;
			TRE::Vector<VkBuffer>		  destroyedBuffers;
			TRE::Vector<VkBufferView>	  destroyedBufferViews;
			TRE::Vector<VkRenderPass>	  destroyedRenderPasses;
			TRE::Vector<VkDescriptorPool> destroyedDescriptorPool;
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

		ImageHandle CreateImage(const ImageCreateInfo& createInfo, const void* data = NULL);

		ImageViewHandle CreateImageView(const ImageViewCreateInfo& createInfo);

		bool CreateBufferInternal(VkBuffer& outBuffer, MemoryView& outMemoryView, const BufferInfo& createInfo);

		BufferHandle CreateBuffer(const BufferInfo& createInfo, const void* data = NULL);

		RingBufferHandle CreateRingBuffer(const BufferInfo& createInfo, const uint32 ringSize = NUM_FRAMES, const void* data = NULL);

		SamplerHandle CreateSampler(const SamplerInfo& createInfo);
	
		CommandBufferHandle RequestCommandBuffer(QueueTypes type);

		void Submit(CommandBufferHandle cmd);

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

		void Init();

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
		AttachmentAllocator								 transientAttachmentAllocator;

		PerFrame		perFrame[MAX_FRAMES];
		HandlePool		objectsPool;

		uint32 msaaSamplerCount;

		friend class Image;
		friend class Swapchain;
	};
};

TRE_NS_END