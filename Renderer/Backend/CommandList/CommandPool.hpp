#pragma once

#include <Renderer/Common.hpp>
#include <vector>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

	class RENDERER_API CommandPool
	{
	public:
        CommandPool() = default;

        CommandPool(RenderDevice* device, uint32 queueFamilyIndex);

		~CommandPool();

        CommandPool(CommandPool&&) = delete;
        CommandPool& operator=(CommandPool&&) = delete;
		CommandPool(const CommandPool&) = delete;
		void operator=(const CommandPool&) = delete;

		void Reset();

		VkCommandBuffer RequestCommandBuffer();

		VkCommandBuffer RequestSecondaryCommandBuffer();

		void Destroy();
	private:
        RenderDevice* device;
		VkCommandPool pool;
		std::vector<VkCommandBuffer> buffers;
		std::vector<VkCommandBuffer> secondaryBuffers;

		uint32 index;
		uint32 secondaryIndex;
	};
}

TRE_NS_END
