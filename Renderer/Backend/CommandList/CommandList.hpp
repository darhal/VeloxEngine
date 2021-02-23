#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Core/Handle/Handle.hpp>
#include <Renderer/Backend/ShaderProgram/ResourceBinding/ResourceBinding.hpp>
#include <Renderer/Backend/Synchronization/Event/Event.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;
    class CommandPool;

	class Buffer;
	class RingBuffer;
	class Image;
	class ImageView;
	class Sampler;
	class DescriptorSetLayout;
	class RenderPass;
	class Framebuffer;
	class ShaderProgram;
	class GraphicsState;
	class Pipeline;
	struct RenderPassInfo;

	class SBT;
	class Tlas;

	struct DescriptorSetDirty
	{
		uint8 sets = 0;
		uint8 dynamicSets = 0;
	};

	struct CommandBufferDeleter
	{
		void operator()(class CommandBuffer* cmd);
	};

	class CommandBuffer : public Utils::RefCounterEnabled<CommandBuffer, CommandBufferDeleter, HandleCounter>
	{
	public:
		enum class Type
		{
			GENERIC = 0,
            ASYNC_TRANSFER = 1,
            ASYNC_COMPUTE = 2,
            RAY_TRACING = 0,

            MAX = 3,
		};
	public:
		friend struct CommandBufferDeleter;
        friend class CommandPool;

        CommandBuffer(RenderDevice& device, CommandPool* pool, VkCommandBuffer buffer, Type type);

        ~CommandBuffer();

        void Reset();

		void Begin();

		void End();

		// Compute dispatch:
		void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ);

		// Graphics settings
		void SetViewport(const VkViewport& viewport);

		void SetScissor(const VkRect2D& scissor);

		void BeginRenderPass(VkClearColorValue clearColor);

		void BeginRenderPass(const RenderPassInfo& info, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

		void EndRenderPass();

		void NextRenderPass(VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

		void BindPipeline(const Pipeline& pipeline);

		void BindVertexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void BindIndexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void DrawIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 firstIndex = 0, int32 vertexOffset = 0, uint32 firstInstance = 0);

		void Draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);

		void BindDescriptorSet(const Pipeline& pipeline, const std::initializer_list<VkDescriptorSet>& descriptors, 
			const std::initializer_list<uint32>& dyncOffsets);

		void BindShaderProgram(ShaderProgram& program);

		// Graphics State functions:
		void SetGraphicsState(GraphicsState& state);


		// Uniform buffers, textures, input attachments, etc
		void SetUniformBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset = 0, DeviceSize range = VK_WHOLE_SIZE);

		void SetStorageBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset = 0, DeviceSize range = VK_WHOLE_SIZE);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture, const Sampler& sampler);

		void SetSampler(uint32 set, uint32 binding, const Sampler& sampler);

		void SetInputAttachments(uint32 set, uint32 startBinding = 0);

		void PushConstants(ShaderStagesFlags stages, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

		// Ray Tracing
		void SetAccelerationStrucure(uint32 set, uint32 binding, const Tlas& tlas);

		void SetAccelerationStrucure(uint32 set, uint32 binding, VkAccelerationStructureKHR* tlas);

		void TraceRays(uint32 width, uint32 height, uint32 depth = 1);

		void TraceRays(VkPipeline pipeline, const SBT& sbt, uint32 width, uint32 height, uint32 depth = 1);

		void CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, DeviceSize size, DeviceSize srcOffset = 0, DeviceSize dstOffset = 0);

		void CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer);

		void CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, const VectorView<VkBufferCopy>& copies);


		void CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, VkDeviceSize bufferOffset, const VkOffset3D& imageOffset,
			const VkExtent3D& imageExtent, uint32_t bufferRowLength, uint32_t bufferImageHeight,
			const VkImageSubresourceLayers& imageSubresource);

		void CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, VkDeviceSize bufferOffset = 0, uint32 mipLevel = 0,
			uint32_t bufferRowLength = 0, uint32_t bufferImageHeight = 0);

		void CopyBufferToImage(const Buffer& srcBuffer, const Image& dstImage, const VectorView<VkBufferImageCopy>& copies);

	
		void CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, VkDeviceSize bufferOffset, const VkOffset3D& imageOffset,
			const VkExtent3D& imageExtent, uint32_t bufferRowLength, uint32_t bufferImageHeight,
			const VkImageSubresourceLayers& imageSubresource);

		void CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, VkDeviceSize bufferOffset = 0, uint32 mipLevel = 0,
			uint32_t bufferRowLength = 0, uint32_t bufferImageHeight = 0);

		void CopyImageToBuffer(const Image& srcImage, const Buffer& dstBuffer, const VectorView<VkBufferImageCopy>& copies);

		void CopyImage(const Image& srcImage, const Image& dstImage, const VkExtent3D& extent, uint32 srcMipLevel = 0, uint32 dstMipLevel = 0,
			const VkOffset3D& srcOffset = {0, 0, 0}, const VkOffset3D& dstOffset = {0, 0, 0});

		void CopyImage(const Image& srcImage, const Image& dstImage, const VectorView<VkImageCopy>& regions);

		// Prepare an image to have its mipmap generated.
		// Puts the top level into TRANSFER_SRC_OPTIMAL, and the all the remaining levels are set to TRANSFER_DST_OPTIMAL
		void PrepareGenerateMipmapBarrier(const Image& image, VkImageLayout baseLevelLayout, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess,
			bool needTopLevelBarrier = true);

		void GenerateMipmap(const Image& image);

		void BlitImage(const Image& dst, const Image& src,
			const VkOffset3D& dstOffset,
			const VkOffset3D& dstExtent, const VkOffset3D& srcOffset, const VkOffset3D& srcExtent,
			uint32 dstLevel, uint32 srcLevel, uint32 dstBaseLayer, uint32 srcBaseLayer,
			uint32 numLayers, VkFilter filter = VK_FILTER_LINEAR);


		EventHandle SignalEvent(VkPipelineStageFlags stages);

		void WaitEvents(EventHandle* event, uint32 eventsCount, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages,
			const VkMemoryBarrier* barriers, uint32 barriersCount, const VkBufferMemoryBarrier* buffersBarriers, uint32 bufferCount,
			const VkImageMemoryBarrier* imagesBarriers, uint32 imageCoun);

		void FullBarrier();

		void PixelBarrier();

		void Barrier(VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages,
			uint32 barriers, const VkMemoryBarrier* globals,
			uint32 bufferBarriers, const VkBufferMemoryBarrier* buffers,
			uint32 imageBarriers, const VkImageMemoryBarrier* images);

		void Barrier(VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

		void BufferBarrier(const Buffer& buffer, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, 
			VkAccessFlags dstAccess);

		void ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, 
			VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

		void ChangeImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		void ChangeImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		FORCEINLINE VkCommandBuffer GetApiObject() const { return commandBuffer; }

		FORCEINLINE Type GetType() const { return type; }

		FORCEINLINE bool UsesSwapchain() const { return renderToSwapchain; }

		FORCEINLINE const RenderPass* GetRenderPass() { return renderPass; }

		FORCEINLINE const Framebuffer* GetFramebuffer() { return framebuffer; }

		// Input Assembly
		void SetPrimitiveTopology(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);

		// Rasterization
		void SetDepthClampEnable(VkBool32 depthClampEnable);

		void SetRasterizerDiscardEnable(VkBool32 rasterizerDiscardEnable);

		void SetPolygonMode(VkPolygonMode polygonMode);

		void SetCullMode(VkCullModeFlags cullMode);

		void SetFrontFrace(VkFrontFace frontFace);

		void SetDepthBias(VkBool32 depthBiasEnable, float depthBiasConstantFactor = 0.f, float depthBiasClamp = 0.f, float depthBiasSlopeFactor = 0.f);

		void SetLineWidth(float lineWidth = 1.f);

		// Depth Stencil state
		void SetDepthTest(VkBool32 depthTestEnable, VkBool32 depthWriteEnable);

		void SetDepthBounds(float minDepthBounds, float maxDepthBounds);

		void SetDepthCompareOp(VkCompareOp depthCompareOp);

		void EnableDepthBoundsTest(VkBool32 depthBoundsTestEnable);

		void EnableStencilTest(VkBool32 stencilTestEnable);

		void SetStencilOpState(VkStencilOpState front, VkStencilOpState back);

		// Blend state
		void SetLogicOp(VkBool32 logicOpEnable, VkLogicOp logicOp = VK_LOGIC_OP_SET);

		void SetBlendConstants(float blendConstants[4]);

		void SetAttachmentBlendFactor(uint32 attach, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor,
			VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor);

		void SetAttachmentBlendOp(uint32 attach, VkBlendOp colorBlendOp, VkBlendOp alphaBlendOp);

		void SetAttachmentColorWriteMask(uint32 attach, VkColorComponentFlags colorWriteMask);
	private:
		void UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings);

		void FlushDescriptorSet(uint32 set);

		void FlushDescriptorSets();

		void RebindDescriptorSet(uint32 set);

		void InitViewportScissor(const RenderPassInfo& info, const Framebuffer* fb);

		void BindPipeline();
	private:
        ResouceBindings bindings;
        DescriptorSetDirty dirty;
        RenderDevice& device;

        CommandPool* pool;
        GraphicsState* state;
        ShaderProgram* program;
        const Pipeline* pipeline;
        const RenderPass* renderPass;
        const Framebuffer* framebuffer;
        const ImageView* framebufferAttachments[MAX_ATTACHMENTS + 1] = {};

        VkDescriptorSet allocatedSets[MAX_DESCRIPTOR_SET];
        VkCommandBuffer commandBuffer;

        VkRect2D scissor;
        VkViewport viewport;

        Type type;

        uint32 subpassIndex;

        bool renderToSwapchain;
        bool stateUpdate;
	};

	typedef CommandBuffer CommandList;
	using CommandBufferHandle = Handle<CommandBuffer>;
}

TRE_NS_END
