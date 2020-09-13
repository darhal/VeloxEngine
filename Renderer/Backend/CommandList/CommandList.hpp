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
	class RenderContext;
	class GraphicsPipeline;
	class DescriptorSetLayout;

	struct DescriptorSetDirty
	{
		uint32 dirtyBindings[MAX_DESCRIPTOR_SET];
		uint8 dirtySets;
	};

	class CommandBuffer
	{
	public:
		enum class Type
		{
			GENERIC,
			ASYNC_GRAPHICS,
			ASYNC_COMPUTE,
			ASYNC_TRANSFER,
			MAX
		};
	public:
		CommandBuffer(RenderContext* renderContext, VkCommandBuffer buffer, Type type);

		void Begin();

		void End();

		void SetViewport(const VkViewport& viewport);

		void SetScissor(const VkRect2D& scissor);

		void BeginRenderPass(VkClearColorValue clearColor);

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

		void Barrier(VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage,
			VkAccessFlags dstAccess);

		void BufferBarrier(const Buffer& buffer, VkPipelineStageFlags srcStage, VkAccessFlags srcAccess,
			VkPipelineStageFlags dstStage, VkAccessFlags dstAccess);

		void ImageBarrier(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout,
			VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage,
			VkAccessFlags dstAccess);

		FORCEINLINE VkCommandBuffer GetAPIObject() const { return commandBuffer; }
	private:
		void UpdateDescriptorSet(uint32 set, VkDescriptorSet descSet, const DescriptorSetLayout& layout, const ResourceBinding* bindings);

		void FlushDescriptorSet(uint32 set);

		void FlushDescriptorSets();
	private:
		ResouceBindings bindings;
		DescriptorSetDirty dirtySets;
		RenderContext* renderContext;
		const GraphicsPipeline* pipeline;
		VkDescriptorSet allocatedSets[MAX_DESCRIPTOR_SET];
		VkCommandBuffer commandBuffer;
		Type type;
	};

	typedef CommandBuffer CommandList;
	using CommandBufferHandle = Handle<CommandBuffer>;
}

TRE_NS_END