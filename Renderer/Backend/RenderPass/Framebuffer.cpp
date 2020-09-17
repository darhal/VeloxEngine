#include "Framebuffer.hpp"

TRE_NS_START

Renderer::Framebuffer::Framebuffer(const RenderPass& rp, const RenderPassInfo& info) :
	framebuffer(VK_NULL_HANDLE),
	renderPass(rp),
	renderPassInfo(info),
	width(0),
	height(0)
{
}



TRE_NS_END
