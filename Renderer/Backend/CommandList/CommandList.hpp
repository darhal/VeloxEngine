#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Core/Handle/Handle.hpp>

TRE_NS_START

namespace Renderer
{
	class Buffer;
	class RenderContext;
	class GraphicsPipeline;

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

		FORCEINLINE VkCommandBuffer GetAPIObject() const { return commandBuffer; }
	private:
		RenderContext* renderContext;
		VkCommandBuffer commandBuffer;
		Type type;
	};

	typedef CommandBuffer CommandList;
	using CommandBufferHandle = Utils::Handle<CommandBuffer>;
}

TRE_NS_END