#include "Fence.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::FenceDeleter::operator()(Fence* fence)
{
    fence->device.GetObjectsPool().fences.Free(fence);
}

Renderer::Fence::~Fence()
{
	if (fence != VK_NULL_HANDLE)
        device.ResetFence(fence, beenWaiting);
}

void Renderer::Fence::Wait(uint64_t timeout)
{
	if (beenWaiting)
		return;

    vkWaitForFences(device.GetDevice(), 1, &fence, VK_TRUE, timeout);
	beenWaiting = true;
}

void Renderer::Fence::Reset()
{
    vkResetFences(device.GetDevice(), 1, &fence);
}

TRE_NS_END

