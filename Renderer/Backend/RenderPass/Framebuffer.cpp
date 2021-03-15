#include "Framebuffer.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::Framebuffer::Framebuffer(const RenderDevice& device, const RenderPass& rp, const RenderPassInfo& info) :
	framebuffer(VK_NULL_HANDLE),
	renderDevice(device),
	renderPass(rp),
	renderPassInfo(info),
	width(0),
	height(0)
{
	VkImageView views[MAX_ATTACHMENTS + 1];
	uint32 viewsCount = 0;

	ComputeDimensions(info, width, height);
	viewsCount = SetupRawViews(views, info);
	uint32 num_layers = info.layersCount > 1 ? (info.layersCount + info.baseLayer) : 1;

	VkFramebufferCreateInfo fb_info;
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = NULL;
	fb_info.flags = 0;
	fb_info.renderPass = rp.GetApiObject();
	fb_info.attachmentCount = viewsCount;
	fb_info.pAttachments = views;
	fb_info.width = width;
	fb_info.height = height;
	fb_info.layers = num_layers;

	// printf("Creating FB\n");
	vkCreateFramebuffer(renderDevice.GetDevice(), &fb_info, NULL, &framebuffer);
}

Renderer::Framebuffer::~Framebuffer()
{
    if (framebuffer) {
        vkDestroyFramebuffer(renderDevice.GetDevice(), framebuffer, NULL);
        framebuffer = VK_NULL_HANDLE;
        // printf("Destroy framebuffer!\n");
    }
}


void Renderer::Framebuffer::ComputeDimensions(const RenderPassInfo& info, uint32_t& width, uint32_t& height)
{
	width = UINT32_MAX;
	height = UINT32_MAX;
	ASSERT(!(info.colorAttachmentCount || info.depthStencil));

	for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
		ASSERT(!info.colorAttachments[i]);
		uint32 lod = info.colorAttachments[i]->GetInfo().baseLevel;
		width = TRE::Math::Min(width, info.colorAttachments[i]->GetImage()->GetWidth(lod));
		height = TRE::Math::Min(height, info.colorAttachments[i]->GetImage()->GetHeight(lod));
	}

	if (info.depthStencil) {
		uint32 lod = info.depthStencil->GetInfo().baseLevel;
		width = TRE::Math::Min(width, info.depthStencil->GetImage()->GetWidth(lod));
		height = TRE::Math::Min(height, info.depthStencil->GetImage()->GetHeight(lod));
	}
}

unsigned Renderer::Framebuffer::SetupRawViews(VkImageView* views, const RenderPassInfo& info)
{
	uint32 num_views = 0;

	for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
		ASSERT(!info.colorAttachments[i]);

		// For multiview, we use view indices to pick right layers.
		if (info.layersCount > 1)
			views[num_views++] = info.colorAttachments[i]->GetApiObject();
		else
			views[num_views++] = info.colorAttachments[i]->GetApiObject();// info.colorAttachments[i]->get_render_target_view(info.base_layer);
	}

	if (info.depthStencil) {
		// For multiview, we use view indices to pick right layers.
		if (info.layersCount > 1)
			views[num_views++] = info.depthStencil->GetApiObject();
		else
			views[num_views++] = info.depthStencil->GetApiObject();//info.depthStencil->get_render_target_view(info.base_layer);
	}

	return num_views;
}


TRE_NS_END


