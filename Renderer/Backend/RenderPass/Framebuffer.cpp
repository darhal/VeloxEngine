#include "Framebuffer.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

Renderer::Framebuffer::Framebuffer(const RenderDevice& device, const RenderPass& rp, const RenderPassInfo& info) :
	framebuffer(VK_NULL_HANDLE),
	renderDevice(device),
	renderPass(rp),
	renderPassInfo(info),
	width(0),
	height(0)
{
	ComputeDimensions(info, width, height);

	VkImageView views[MAX_ATTACHMENTS + 1];
	uint32 viewsCount = 0;

	viewsCount = SetupRawViews(views, info);

	VkFramebufferCreateInfo fb_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	VkFramebufferAttachmentsCreateInfoKHR attachments_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO_KHR };
	fb_info.renderPass = rp.GetAPIObject();
	fb_info.attachmentCount = viewsCount;

	unsigned num_layers = info.layersCount > 1 ? (info.layersCount + info.baseLayer) : 1;
	VkFormat view_formats[MAX_ATTACHMENTS][2];
	VkFramebufferAttachmentImageInfoKHR image_infos[MAX_ATTACHMENTS + 1];

	fb_info.pAttachments = views;

	fb_info.width = width;
	fb_info.height = height;
	fb_info.layers = num_layers;

	vkCreateFramebuffer(renderDevice.GetDevice(), &fb_info, NULL, &framebuffer);
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
		height = TRE::Math::Min(height, info.depthStencil->GetImage()->GetWidth(lod));
	}
}

unsigned Renderer::Framebuffer::SetupRawViews(VkImageView* views, const RenderPassInfo& info)
{
	uint32 num_views = 0;

	for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
		ASSERT(!info.colorAttachments[i]);

		// For multiview, we use view indices to pick right layers.
		if (info.layersCount > 1)
			views[num_views++] = info.colorAttachments[i]->GetAPIObject();
		else
			views[num_views++] = info.colorAttachments[i]->GetAPIObject();// info.colorAttachments[i]->get_render_target_view(info.base_layer);
	}

	if (info.depthStencil) {
		// For multiview, we use view indices to pick right layers.
		if (info.layersCount > 1)
			views[num_views++] = info.depthStencil->GetAPIObject();
		else
			views[num_views++] = info.depthStencil->GetAPIObject();//info.depthStencil->get_render_target_view(info.base_layer);
	}

	return num_views;
}

TRE_NS_END


