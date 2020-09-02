#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	class Buffer;

	class CommandList
	{
	public:
		void BeginRenderPass();

		void EndRenderPass();

		void SetVertexBuffer(Buffer& buffer);

		void SetIndexBuffer(Buffer& buffer);
	private:
		VkCommandBuffer commandBuffer;

	};

	typedef CommandList CommandBuffer;
}

TRE_NS_END