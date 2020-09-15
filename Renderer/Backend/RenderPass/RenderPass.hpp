#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class ImageView;
	class RenderBackend;

	struct RenderPassInfo
	{
		const ImageView* colorAttachments[MAX_ATTACHMENTS];
		const ImageView* depthStencil = NULL;
		uint32 colorAttachmentCount = 0;

		uint32 clearAttachments = 0;
		uint32 loadAttachments = 0;
		uint32 storeAttachments = 0;

		VkRect2D renderArea = { { 0, 0 }, { UINT32_MAX, UINT32_MAX } };

		VkClearColorValue clear_color[MAX_ATTACHMENTS] = {};
		VkClearDepthStencilValue clear_depth_stencil = { 1.0f, 0 };

		enum class DepthStencil
		{
			NONE,
			READ_ONLY,
			READ_WRITE
		};

		struct Subpass
		{
			uint32 colorAttachments[MAX_ATTACHMENTS];
			uint32 inputAttachments[MAX_ATTACHMENTS];
			uint32 resolveAttachments[MAX_ATTACHMENTS];
			uint32 colorAttachmentsCount = 0;
			uint32 inputAttachmentsCount = 0;
			uint32 resolveAttachmentsCount = 0;
			DepthStencil depth_stencil_mode = DepthStencil::READ_WRITE;
		};

		// If 0/nullptr, assume a default subpass.
		const Subpass* subpasses = NULL;
		uint32 subpassesCount = 0;
	};

	class RenderPass
	{
	public:
		struct SubpassInfo
		{
			VkAttachmentReference colorAttachments[MAX_ATTACHMENTS];
			VkAttachmentReference inputAttachments[MAX_ATTACHMENTS];
			VkAttachmentReference depthStencilAttachment;
			uint32 colorAttachmentsCount;
			uint32 inputAttachmentsCount;
			uint32 samples;
		};

		RenderPass(const RenderBackend& backend, const RenderPassInfo& info);
	private:
		VkRenderPass renderPass;
		std::vector<SubpassInfo> subpassesInfo;
	};
}

TRE_NS_END