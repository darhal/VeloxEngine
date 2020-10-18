#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Core/Handle/Handle.hpp>
#include <Renderer/Backend/ShaderProgram/ResourceBinding/ResourceBinding.hpp>

TRE_NS_START

namespace Renderer
{
	class Buffer;
	class Image;
	class ImageView;
	class Sampler;
	class RenderBackend;
	class GraphicsPipeline;
	class DescriptorSetLayout;
	class RenderPass;
	class Framebuffer;
	struct RenderPassInfo;

	struct DescriptorSetDirty
	{
		uint32 dirtyBindings[MAX_DESCRIPTOR_SET];
		uint8 dirtySets;
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

		void BindPipeline(const GraphicsPipeline& pipeline);

		void BindVertexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void BindIndexBuffer(const Buffer& buffer, DeviceSize offset = 0);

		void DrawIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 firstIndex = 0, int32 vertexOffset = 0, uint32 firstInstance = 0);

		void Draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);

		void BindDescriptorSet(const GraphicsPipeline& pipeline, const std::initializer_list<VkDescriptorSet>& descriptors, 
			const std::initializer_list<uint32>& dyncOffsets);

		void SetUniformBuffer(uint32 set, uint32 binding, const Buffer& buffer, DeviceSize offset, DeviceSize range = VK_WHOLE_SIZE);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture);

		void SetSampler(uint32 set, uint32 binding, const Sampler& sampler);

		void SetTexture(uint32 set, uint32 binding, const ImageView& texture, const Sampler& sampler);

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

		void Barrier(VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages,
			uint32 barriers, const VkMemoryBarrier* globals,
			uint32 bufferBarriers, const VkBufferMemoryBarrier* buffers,
			uint32 imageBarriers, const VkImageMemoryBarrier* images);

		void Barrier(VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage,
			VkAccessFlags dstAccess);

		void BufferBarrier(const Buffer& buffer, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess,
			VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

		void ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout,
			VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage,
			VkAccessFlags dstAccess);

		FORCEINLINE VkCommandBuffer GetAPIObject() const { return commandBuffer; }

		FORCEINLINE Type GetType() const { return type; }

		FORCEINLINE bool UsesSwapchain() const { return renderToSwapchain; }
	private:
		void UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings);

		void FlushDescriptorSet(uint32 set);

		void FlushDescriptorSets();

		void InitViewportScissor(const RenderPassInfo& info, const Framebuffer* fb);
	private:
		ResouceBindings bindings;
		DescriptorSetDirty dirtySets;
		RenderBackend* renderBackend;

		const GraphicsPipeline* pipeline;
		const RenderPass* renderPass;
		const Framebuffer* framebuffer;

		VkRect2D scissor;
		VkViewport viewport;

		VkDescriptorSet allocatedSets[MAX_DESCRIPTOR_SET];
		VkCommandBuffer commandBuffer;
		Type type;

		bool renderToSwapchain;
	};

	typedef CommandBuffer CommandList;
	using CommandBufferHandle = Handle<CommandBuffer>;
}

TRE_NS_END