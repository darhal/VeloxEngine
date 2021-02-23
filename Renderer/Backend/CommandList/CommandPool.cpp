#include "CommandPool.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::CommandPoolDeleter::operator()(Renderer::CommandPool* pool)
{
    pool->device->GetObjectsPool().commandPools.Free(pool);
}

Renderer::CommandPool::CommandPool(RenderDevice* device, uint32 queueFamilyIndex, CommandBuffer::Type cmdType, uint32 type) :
    device(device), type(type), cmdType(cmdType), index(0), secondaryIndex(0)
{
	VkCommandPoolCreateInfo info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    info.flags            = type;
	info.queueFamilyIndex = queueFamilyIndex;

    vkCreateCommandPool(device->GetDevice(), &info, NULL, &pool);
}


Renderer::CommandPool::~CommandPool()
{
    this->Destroy();
}


void Renderer::CommandPool::Reset()
{
    if (type & VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) {
        vkResetCommandPool(device->GetDevice(), pool, 0);
    }

    index = 0;
    secondaryIndex = 0;
}


Renderer::CommandBufferHandle Renderer::CommandPool::RequestCommandBuffer()
{
    VkCommandBuffer cmd;

    if (index < freeHandles.size() && (type & VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) {
        CommandBufferHandle handle = freeHandles[index++];
        handle->Reset();
        handle->Begin();
        return handle;
    }else{
        VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        vkAllocateCommandBuffers(device->GetDevice(), &info, &cmd);
        index++;
    }

    CommandBufferHandle handle(commandBuffers.Allocate(*device, this, cmd, cmdType));
    freeHandles.push_back(handle);
    handle->Begin();
    return handle;
}


Renderer::CommandBufferHandle Renderer::CommandPool::RequestSecondaryCommandBuffer()
{
    VkCommandBuffer cmd;

    if (secondaryIndex < freeSecondaryHandles.size() && (type & VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) {
        CommandBufferHandle handle = freeSecondaryHandles[secondaryIndex++];
        handle->Reset();
        handle->Begin();
        return handle;
    }else{
        VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;
        vkAllocateCommandBuffers(device->GetDevice(), &info, &cmd);
        secondaryIndex++;
    }

    CommandBufferHandle handle(commandBuffers.Allocate(*device, this, cmd, cmdType));
    freeSecondaryHandles.push_back(handle);
    handle->Begin();
    return handle;
}


void Renderer::CommandPool::Destroy()
{
    if (pool != VK_NULL_HANDLE) {
        commandBuffers.Clear();
        secondaryCommandBuffers.Clear();
        freeHandles.clear();
        freeSecondaryHandles.clear();

        if (pool != VK_NULL_HANDLE) {
            device->DestroyCommandPool(pool);
        }

        pool = VK_NULL_HANDLE;
    }
}

void Renderer::CommandPool::Free(Renderer::CommandBuffer* cmd)
{
    commandBuffers.Free(cmd);
}

void Renderer::CommandPool::Recycle(VkCommandBuffer cmd)
{
    // freeCommandBuffers.emplace_back(cmd);
}

void Renderer::CommandPool::RecycleSecondary(VkCommandBuffer cmd)
{
    // freeSecondaryCommandBuffers.emplace_back(cmd);
}

TRE_NS_END

