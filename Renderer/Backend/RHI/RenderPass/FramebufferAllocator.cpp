#include "FramebufferAllocator.hpp"
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/RHI/RenderPass/Framebuffer.hpp>
#include <Renderer/Backend/RHI/RenderPass/RenderPass.hpp>
#include <Renderer/Backend/RHI/Images/Image.hpp>

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
		// h.u64(uint64(info.colorAttachments[i])); // this caused a bug!
		h.Data(info.colorAttachments[i]->GetInfo());
	}

	if (info.depthStencil) {
        // h.u64(uint64(info.depthStencil)); // this caused a bug !
        h.Data(info.depthStencil->GetInfo());
	}

	h.u32(info.baseLayer);
	auto hash = h.Get();

	/*auto fb = framebufferCache.find(hash);

	if (fb != framebufferCache.end()) {
		// printf("Getting framebuffer ID: %llu.\n", hash);
		return fb->second;
	}

	auto fb2 = framebufferCache.emplace(hash, Framebuffer(*renderDevice, renderPass, info));
	printf("Creating framebuffer ID: %llu.\n", hash);
	return fb2.first->second;*/

    // printf("Fetching framebuffer ID: %llu.\n", hash);
	FramebufferNode* node = framebufferCache.RequestEmplace(hash, *renderDevice, renderPass, info);
	return *node;
}

void Renderer::FramebufferAllocator::BeginFrame()
{
	framebufferCache.BeginFrame();
}

void Renderer::FramebufferAllocator::Clear()
{
	framebufferCache.Clear();
}

void Renderer::FramebufferAllocator::Destroy()
{
    framebufferCache.Clear();
}


TRE_NS_END


