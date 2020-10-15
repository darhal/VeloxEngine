#include "Fence.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

void Renderer::FenceDeleter::operator()(Fence* fence)
{
	fence->backend.GetObjectsPool().fences.Free(fence);
}

Renderer::Fence::~Fence()
{
	if (fence != VK_NULL_HANDLE)
		backend.ResetFence(fence, beenWaiting);
}

void Renderer::Fence::Wait(uint64_t timeout)
{
	if (beenWaiting)
		return;

	vkWaitForFences(backend.GetRenderDevice().GetDevice(), 1, &fence, VK_TRUE, timeout);
	beenWaiting = true;
}

void Renderer::Fence::Reset()
{
	vkResetFences(backend.GetRenderDevice().GetDevice(), 1, &fence);
}

TRE_NS_END

