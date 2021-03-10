#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/CommandList/CommandList.hpp>
#include <vector>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

    struct CommandPoolDeleter
    {
        void operator()(class CommandPool* pool);
    };

    class RENDERER_API CommandPool : public Utils::RefCounterEnabled<CommandPool, CommandPoolDeleter, HandleCounter>
	{
	public:
        enum Type
        {
            NONE = 0,
            TRANSIENT = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            CMD_BUFF_RESET = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        };

        friend struct CommandPoolDeleter;

        CommandPool() : device(NULL), pool(VK_NULL_HANDLE) {};

        CommandPool(RenderDevice* device, uint32 queueFamilyIndex, CommandBuffer::Type cmdType = CommandBuffer::Type::GENERIC,
                    uint32 type = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

		~CommandPool();

        void Reset();

        CommandBufferHandle RequestCommandBuffer();

        CommandBufferHandle RequestSecondaryCommandBuffer();

		void Destroy();

        void Free(CommandBuffer* cmd);

        void Recycle(VkCommandBuffer cmd);

        void RecycleSecondary(VkCommandBuffer cmd);

        uint32 GetType() const { return type; }

        FORCEINLINE VkCommandPool GetApiObject() const { return pool; }

        CommandPool(CommandPool&&) = delete;
        CommandPool& operator=(CommandPool&&) = delete;
        CommandPool(const CommandPool&) = delete;
        void operator=(const CommandPool&) = delete;
	private:
        RenderDevice* device;
		VkCommandPool pool;
        uint32 type;
        CommandBuffer::Type cmdType;

        ObjectPool<CommandBuffer> commandBuffers;
        ObjectPool<CommandBuffer> secondaryCommandBuffers;

        std::vector<CommandBufferHandle> freeHandles;
        std::vector<CommandBufferHandle> freeSecondaryHandles;
        uint32 index, secondaryIndex;
	};

    using CommandPoolHandle = Handle<CommandPool>;
}

TRE_NS_END
