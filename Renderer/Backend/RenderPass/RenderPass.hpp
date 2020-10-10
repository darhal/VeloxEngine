#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class ImageView;
	class RenderDevice;

	enum RenderPassOp
	{
		RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT = 1 << 0,
		RENDER_PASS_OP_LOAD_DEPTH_STENCIL_BIT = 1 << 1,
		RENDER_PASS_OP_STORE_DEPTH_STENCIL_BIT = 1 << 2,
		RENDER_PASS_OP_DEPTH_STENCIL_READ_ONLY_BIT = 1 << 3,
		RENDER_PASS_OP_ENABLE_TRANSIENT_STORE_BIT = 1 << 4,
		RENDER_PASS_OP_ENABLE_TRANSIENT_LOAD_BIT = 1 << 5
	};

	struct RenderPassInfo
	{
		const ImageView* colorAttachments[MAX_ATTACHMENTS];
		const ImageView* depthStencil = NULL;
		uint32 colorAttachmentCount = 0;

		uint32 clearAttachments = 0;
		uint32 loadAttachments = 0;
		uint32 storeAttachments = 0;
		uint32 baseLayer = 0;
		uint32 layersCount = 1;

		uint32 opFlags = 0;

		VkRect2D renderArea = { { 0, 0 }, { UINT32_MAX, UINT32_MAX } };

		VkClearColorValue clearColor[MAX_ATTACHMENTS] = {};
		VkClearDepthStencilValue clearDepthStencil = { 1.0f, 0 };

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
			DepthStencil depthStencilMode = DepthStencil::READ_WRITE;
		};

		// If 0/NULL, assume a default subpass.
		Subpass* subpasses = NULL;
		uint32 subpassesCount = 0;
	};

	class RenderPass : public Hashable
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

		RenderPass(const RenderDevice& device, const RenderPassInfo& info);

		VkRenderPass GetAPIObject() const { return renderPass; }
	private:
		void SetupRenderPass(const VkRenderPassCreateInfo& createInfo);
	private:
		VkRenderPass renderPass;
		std::vector<SubpassInfo> subpassesInfo;

		VkFormat colorAttachmentsFormat[MAX_ATTACHMENTS] = {};
		VkFormat depthStencilFormat;

		friend class RenderBackend;
	};
}

TRE_NS_END