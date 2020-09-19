#include "FramebufferAllocator.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RenderPass/Framebuffer.hpp>
#include <Renderer/Backend/RenderPass/RenderPass.hpp>

TRE_NS_START

Renderer::FramebufferAllocator::FramebufferAllocator(RenderDevice* device) : 
	renderDevice(device)
{
}

Renderer::Framebuffer& Renderer::FramebufferAllocator::RequestFramebuffer(const RenderPass& renderPass, const RenderPassInfo& info)
{
	Hasher h;
	h.u64(renderPass.GetHash());

	for (unsigned i = 0; i < info.colorAttachmentCount; i++) {
		ASSERT(!info.colorAttachments[i]);
		h.u64(uint64(info.colorAttachments[i]));
	}

	if (info.depthStencil)
		h.u64(uint64(info.depthStencil));

	h.u32(info.baseLayer);
	auto hash = h.Get();

	auto fb = framebufferCache.find(hash);

	if (fb != framebufferCache.end()) {
		return fb->second;
	}

	auto fb2 = framebufferCache.emplace(hash, Framebuffer(*renderDevice, renderPass, info));
	printf("Creating framebuffer ID: %llu.\n", hash);
	return fb2.first->second;
}

void Renderer::FramebufferAllocator::Clear()
{
}


TRE_NS_END

