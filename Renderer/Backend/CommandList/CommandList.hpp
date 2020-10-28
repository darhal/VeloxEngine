#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Core/Handle/Handle.hpp>
#include <Renderer/Backend/ShaderProgram/ResourceBinding/ResourceBinding.hpp>
#include <Renderer/Backend/Synchronization/Event/Event.hpp>

TRE_NS_START

namespace Renderer
{
	class Buffer;
	class RingBuffer;
	class Image;
	class ImageView;
	class Sampler;
	class RenderBackend;
	class GraphicsPipeline;
	class DescriptorSetLayout;
	class RenderPass;
	class Framebuffer;
	class ShaderProgram;
	class GraphicsState;
	struct RenderPassInfo;

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
			GENERIC,
			ASYNC_TRANSFER,
			ASYNC_COMPUTE,
			MAX
		};
	public:
		friend struct CommandBufferDeleter;

		CommandBuffer(RenderBackend* backend, VkCommandBuffer buffer, Type type);

		void Begin();

		void End();

		void SetViewport(const VkViewport& viewport);

		void SetScissor(const VkRect2D& scissor);

		void BeginRenderPass(VkClearColorValue clearColor);

		void BeginRenderPass(const RenderPassInfo& info, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

		void EndRenderPass();

		void NextRenderPass(VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

		void BindPipeline(const GraphicsPipeline& pipeline);

		void BindVertexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void BindIndexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void DrawIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 firstIndex = 0, int32 vertexOffset = 0, uint32 firstInstance = 0);

		void Draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);

		void BindDescriptorSet(const GraphicsPipeline& pipeline, const std::initializer_list<VkDescriptorSet>& descriptors, 
			const std::initializer_list<uint32>& dyncOffsets);

		void BindShaderProgram(const ShaderProgram& program);

		// Graphics State functions:
		void SetGraphicsState(GraphicsState& state);

		// Uniform buffers, textures, input attachments, etc
		void SetUniformBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset = 0, DeviceSize range = VK_WHOLE_SIZE);

		void SetUniformBuffer(uint32 set, uint32 binding, const RingBuffer& buffer, DeviceSize offset = 0, DeviceSize range = VK_WHOLE_SIZE);

		void SetStorageBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset = 0, DeviceSize range = VK_WHOLE_SIZE);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture, const Sampler& sampler);

		void SetSampler(uint32 set, uint32 binding, const Sampler& sampler);

		void SetInputAttachments(uint32 set, uint32 startBinding = 0);

		void PushConstants(ShaderStagesFlags stages, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);


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


		FORCEINLINE VkCommandBuffer GetAPIObject() const { return commandBuffer; }

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
		RenderBackend* renderBackend;

		GraphicsState* state;
		const ShaderProgram* program;
		const GraphicsPipeline* pipeline;
		const RenderPass* renderPass;
		const Framebuffer* framebuffer;
		const ImageView* framebufferAttachments[MAX_ATTACHMENTS + 1] = {};

		VkRect2D scissor;
		VkViewport viewport;

		VkDescriptorSet allocatedSets[MAX_DESCRIPTOR_SET];
		VkCommandBuffer commandBuffer;
		Type type;

		uint32 subpassIndex;

		bool renderToSwapchain;
		bool stateUpdate;
	};

	typedef CommandBuffer CommandList;
	using CommandBufferHandle = Handle<CommandBuffer>;
}

TRE_NS_END