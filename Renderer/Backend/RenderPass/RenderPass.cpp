#include "RenderPass.hpp"
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

VkAttachmentReference* FindColor(const VkSubpassDescription* subpasses, uint32 subpass, uint32 attachment)
{
	auto* colors = subpasses[subpass].pColorAttachments;
	for (uint32 i = 0; i < subpasses[subpass].colorAttachmentCount; i++)
		if (colors[i].attachment == attachment)
			return const_cast<VkAttachmentReference*>(&colors[i]);
	return NULL;
};

VkAttachmentReference* FindResolve(const VkSubpassDescription* subpasses, uint32 subpass, uint32 attachment)
{
	if (!subpasses[subpass].pResolveAttachments)
		return nullptr;

	auto* resolves = subpasses[subpass].pResolveAttachments;
	for (uint32 i = 0; i < subpasses[subpass].colorAttachmentCount; i++)
		if (resolves[i].attachment == attachment)
			return const_cast<VkAttachmentReference*>(&resolves[i]);
	return NULL;
};

VkAttachmentReference* FindInput(const VkSubpassDescription* subpasses, uint32 subpass, uint32 attachment)
{
	auto* inputs = subpasses[subpass].pInputAttachments;
	for (uint32 i = 0; i < subpasses[subpass].inputAttachmentCount; i++)
		if (inputs[i].attachment == attachment)
			return const_cast<VkAttachmentReference*>(&inputs[i]);
	return NULL;
};

VkAttachmentReference* FindDepthStencil(const VkSubpassDescription* subpasses, uint32 subpass, uint32 attachment)
{
	if (subpasses[subpass].pDepthStencilAttachment->attachment == attachment)
		return const_cast<VkAttachmentReference*>(subpasses[subpass].pDepthStencilAttachment);
	else
		return NULL;
};

FORCEINLINE VkAttachmentLoadOp GetColorLoadOp(const Renderer::RenderPassInfo& info, uint32 index)
{
	if ((info.clearAttachments & (1u << index)) != 0)
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	else if ((info.loadAttachments & (1u << index)) != 0)
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	else
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

FORCEINLINE VkAttachmentStoreOp GetColorStoreOp(const Renderer::RenderPassInfo& info, uint32 index)
{
	if ((info.storeAttachments & (1u << index)) != 0)
		return VK_ATTACHMENT_STORE_OP_STORE;
	else
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

Renderer::RenderPass::RenderPass(const RenderDevice& device, const RenderPassInfo& info)
{
	RenderPassInfo::Subpass* subpassInfos = info.subpasses;
	uint32 subpassesCount = info.subpassesCount;
	RenderPassInfo::Subpass defaultSubpassInfo;

	ASSERT(subpassesCount > 32);

	if (!info.subpasses || !info.subpassesCount) {
		defaultSubpassInfo.colorAttachmentsCount = info.colorAttachmentCount;

		for (uint32 i = 0; i < info.colorAttachmentCount; i++)
			defaultSubpassInfo.colorAttachments[i] = i;

		subpassesCount = 1;
		subpassInfos = &defaultSubpassInfo;
	}

	////////////////////////////////////////////////
	/// color attachments :
	////////////////////////////////////////////////

	// Want to make load/store to transient a very explicit thing to do, since it will kill performance.
	bool enable_transient_store = (info.opFlags & RENDER_PASS_OP_ENABLE_TRANSIENT_STORE_BIT) != 0;
	bool enable_transient_load = (info.opFlags & RENDER_PASS_OP_ENABLE_TRANSIENT_LOAD_BIT) != 0;
	uint32 implicit_transitions = 0;
	uint32 implicit_bottom_of_pipe = 0;

	const uint32 attachmentsCount = info.colorAttachmentCount + (info.depthStencil ? 1 : 0);
	VkAttachmentDescription attachments[MAX_ATTACHMENTS + 1];

	for (uint32 i = 0; i < info.colorAttachmentCount; i++) {
		ASSERT(!info.colorAttachments[i]);

		colorAttachmentsFormat[i] = info.colorAttachments[i]->GetInfo().format;
		const auto& image = info.colorAttachments[i]->GetImage();
		auto& att = attachments[i];
		att.flags = 0;
		att.format = colorAttachmentsFormat[i];
		att.samples = image->GetInfo().samples;
		att.loadOp = GetColorLoadOp(info, i);
		att.storeOp = GetColorStoreOp(info, i);
		att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// Undefined final layout here for now means that we will just use the layout of the last
		// subpass which uses this attachment to avoid any dummy transition at the end.
		att.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (image->GetInfo().domain == ImageDomain::TRANSIENT) {
			if (enable_transient_load) {
				att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//info.colorAttachments[i]->GetImage()->GetInfo().layout;
			} else {
				ASSERT(att.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD);
				att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			if (!enable_transient_store) {
				att.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}

			implicit_transitions |= 1u << i;
		} else if (image->IsSwapchainImage()) { // Probably handle swapchain images ?
			if (att.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
				att.initialLayout = image->GetSwapchainLayout();
			else
				att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			att.finalLayout = image->GetSwapchainLayout();

			// If we transition from PRESENT_SRC_KHR, this came from an implicit external subpass dependency
			// which happens in BOTTOM_OF_PIPE. To properly transition away from it, we must wait for BOTTOM_OF_PIPE,
			// without any memory barriers, since memory has been made available in the implicit barrier.
			if (att.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
				implicit_bottom_of_pipe |= 1u << i;
			implicit_transitions |= 1u << i;
		} else {
			att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//image->GetInfo().layout;
		}
	}

	////////////////////////////////////////////////
	/// Depth stencil :
	////////////////////////////////////////////////
	VkAttachmentLoadOp dsLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	VkAttachmentStoreOp dsStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	if (info.opFlags & RENDER_PASS_OP_CLEAR_DEPTH_STENCIL_BIT) {
		dsLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	} else if (info.opFlags & RENDER_PASS_OP_LOAD_DEPTH_STENCIL_BIT) {
		dsLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	}

	if (info.opFlags & RENDER_PASS_OP_STORE_DEPTH_STENCIL_BIT) {
		dsStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	}

	bool dsReadOnly = (info.opFlags & RENDER_PASS_OP_DEPTH_STENCIL_READ_ONLY_BIT) != 0;
	VkImageLayout depthStencilLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	if (info.depthStencil) {
		depthStencilFormat = info.depthStencil->GetInfo().format;
		depthStencilLayout = dsReadOnly ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		const auto& image = info.depthStencil->GetImage();
		auto& att = attachments[info.colorAttachmentCount];
		att.flags = 0;
		att.format = depthStencilFormat;
		att.samples = image->GetInfo().samples;
		att.loadOp = dsLoadOp;
		att.storeOp = dsStoreOp;
		// Undefined final layout here for now means that we will just use the layout of the last
		// subpass which uses this attachment to avoid any dummy transition at the end.
		att.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (FormatToAspectMask(depthStencilFormat) & VK_IMAGE_ASPECT_STENCIL_BIT) {
			att.stencilLoadOp = dsLoadOp;
			att.stencilStoreOp = dsStoreOp;
		} else {
			att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		if (image->GetInfo().domain == ImageDomain::TRANSIENT) {
			if (enable_transient_load) {
				// The transient will behave like a normal image.
				att.initialLayout = depthStencilLayout;
			} else {
				if (att.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
					att.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				if (att.stencilLoadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
					att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

				// For transient attachments we force the layouts.
				att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			if (!enable_transient_store) {
				att.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}

			implicit_transitions |= 1u << info.colorAttachmentCount;
		} else {
			att.initialLayout = depthStencilLayout;
		}
	} else {
		depthStencilFormat = VK_FORMAT_UNDEFINED;
	}


	StackAlloc<VkAttachmentReference, 512> referenceAlloc;
	StackAlloc<uint32, 512> preserveAlloc;

	std::vector<VkSubpassDescription> subpasses(subpassesCount);
	std::vector<VkSubpassDependency> externalDependencies;

	for (uint32 i = 0; i < subpassesCount; i++) {
		auto* colors = referenceAlloc.AllocateInit(subpassInfos[i].colorAttachmentsCount, {});
		auto* inputs = referenceAlloc.AllocateInit(subpassInfos[i].inputAttachmentsCount, {});
		auto* resolves = referenceAlloc.AllocateInit(subpassInfos[i].resolveAttachmentsCount, {});
		auto* depth = referenceAlloc.AllocateInit(1, {});

		auto& subpass = subpasses[i];
		subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount	= subpassInfos[i].colorAttachmentsCount;
		subpass.pColorAttachments		= colors;
		subpass.inputAttachmentCount	= subpassInfos[i].inputAttachmentsCount;
		subpass.pInputAttachments		= inputs;
		subpass.pDepthStencilAttachment = depth;

		if (subpassInfos[i].resolveAttachmentsCount) {
			ASSERT(subpassInfos[i].colorAttachmentsCount != subpassInfos[i].resolveAttachmentsCount);
			subpass.pResolveAttachments = resolves;
		}

		for (uint32 j = 0; j < subpass.colorAttachmentCount; j++) {
			auto att = subpassInfos[i].colorAttachments[j];
			ASSERT(!(att == VK_ATTACHMENT_UNUSED || (att < attachmentsCount)));
			colors[j].attachment = att;
			colors[j].layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}

		for (uint32 j = 0; j < subpass.inputAttachmentCount; j++) {
			auto att = subpassInfos[i].inputAttachments[j];
			ASSERT(!(att == VK_ATTACHMENT_UNUSED || (att < attachmentsCount)));
			inputs[j].attachment = att;
			inputs[j].layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}

		if (subpass.pResolveAttachments) {
			for (uint32 j = 0; j < subpass.colorAttachmentCount; j++) {
				auto att = subpassInfos[i].resolveAttachments[j];
				ASSERT(!(att == VK_ATTACHMENT_UNUSED || (att < attachmentsCount)));
				resolves[j].attachment = att;
				resolves[j].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
		}

		if (info.depthStencil && subpassInfos[i].depthStencilMode != RenderPassInfo::DepthStencil::NONE) {
			depth->attachment = info.colorAttachmentCount;
			// Fill in later.
			depth->layout = VK_IMAGE_LAYOUT_UNDEFINED;
		} else {
			depth->attachment = VK_ATTACHMENT_UNUSED;
			depth->layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	// Now, figure out how each attachment is used throughout the subpasses.
	// Either we don't care (inherit previous pass), or we need something specific.
	// Start with initial layouts.
	uint32_t preserve_masks[MAX_ATTACHMENTS + 1] = {};

	// Last subpass which makes use of an attachment.
	unsigned last_subpass_for_attachment[MAX_ATTACHMENTS + 1] = {};

	// 1 << subpass bit set if there are color attachment self-dependencies in the subpass.
	uint32_t color_self_dependencies = 0;
	// 1 << subpass bit set if there are depth-stencil attachment self-dependencies in the subpass.
	uint32_t depth_self_dependencies = 0;

	// 1 << subpass bit set if any input attachment is read in the subpass.
	uint32_t input_attachment_read = 0;
	uint32_t color_attachment_read_write = 0;
	uint32_t depth_stencil_attachment_write = 0;
	uint32_t depth_stencil_attachment_read = 0;

	uint32_t external_color_dependencies = 0;
	uint32_t external_depth_dependencies = 0;
	uint32_t external_input_dependencies = 0;
	uint32_t external_bottom_of_pipe_dependencies = 0;

	for (unsigned attachment = 0; attachment < attachmentsCount; attachment++) {
		bool used = false;
		auto current_layout = attachments[attachment].initialLayout;

		for (unsigned subpass = 0; subpass < subpassesCount; subpass++) {
			auto* color = FindColor(subpasses.data(), subpass, attachment);
			auto* resolve = FindResolve(subpasses.data(), subpass, attachment);
			auto* input = FindInput(subpasses.data(), subpass, attachment);
			auto* depth = FindDepthStencil(subpasses.data(), subpass, attachment);

			// Sanity check.
			if (color || resolve)
				ASSERT(depth);
			if (depth)
				ASSERT(color || resolve);
			if (resolve)
				ASSERT(color || depth);

			if (!color && !input && !depth && !resolve) {
				if (used)
					preserve_masks[attachment] |= 1u << subpass;
				continue;
			}

			if (!used && (implicit_transitions & (1u << attachment))) {
				// This is the first subpass we need implicit transitions.
				if (color)
					external_color_dependencies |= 1u << subpass;
				if (depth)
					external_depth_dependencies |= 1u << subpass;
				if (input)
					external_input_dependencies |= 1u << subpass;
			}

			if (!used && (implicit_bottom_of_pipe & (1u << attachment)))
				external_bottom_of_pipe_dependencies |= 1u << subpass;

			if (resolve && input) // If used as both resolve attachment and input attachment in same subpass, need GENERAL.
			{
				current_layout = VK_IMAGE_LAYOUT_GENERAL;
				resolve->layout = current_layout;
				input->layout = current_layout;

				// If the attachment is first used as a feedback attachment, the initial layout should actually be GENERAL.
				if (!used && attachments[attachment].initialLayout != VK_IMAGE_LAYOUT_UNDEFINED)
					attachments[attachment].initialLayout = current_layout;

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout) {
					external_color_dependencies |= 1u << subpass;
					external_input_dependencies |= 1u << subpass;
				}

				used = true;
				last_subpass_for_attachment[attachment] = subpass;

				color_attachment_read_write |= 1u << subpass;
				input_attachment_read |= 1u << subpass;
			} else if (resolve) {
				if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
					current_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout)
					external_color_dependencies |= 1u << subpass;

				resolve->layout = current_layout;
				used = true;
				last_subpass_for_attachment[attachment] = subpass;
				color_attachment_read_write |= 1u << subpass;
			} else if (color && input) // If used as both input attachment and color attachment in same subpass, need GENERAL.
			{
				current_layout = VK_IMAGE_LAYOUT_GENERAL;
				color->layout = current_layout;
				input->layout = current_layout;

				// If the attachment is first used as a feedback attachment, the initial layout should actually be GENERAL.
				if (!used && attachments[attachment].initialLayout != VK_IMAGE_LAYOUT_UNDEFINED)
					attachments[attachment].initialLayout = current_layout;

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout) {
					external_color_dependencies |= 1u << subpass;
					external_input_dependencies |= 1u << subpass;
				}

				used = true;
				last_subpass_for_attachment[attachment] = subpass;
				color_self_dependencies |= 1u << subpass;

				color_attachment_read_write |= 1u << subpass;
				input_attachment_read |= 1u << subpass;
			} else if (color) // No particular preference
			{
				if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
					current_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				color->layout = current_layout;

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout)
					external_color_dependencies |= 1u << subpass;

				used = true;
				last_subpass_for_attachment[attachment] = subpass;
				color_attachment_read_write |= 1u << subpass;
			} else if (depth && input) // Depends on the depth mode
			{
				ASSERT(subpassInfos[subpass].depthStencilMode == RenderPassInfo::DepthStencil::NONE);
				if (subpassInfos[subpass].depthStencilMode == RenderPassInfo::DepthStencil::READ_WRITE) {
					depth_self_dependencies |= 1u << subpass;
					current_layout = VK_IMAGE_LAYOUT_GENERAL;
					depth_stencil_attachment_write |= 1u << subpass;

					// If the attachment is first used as a feedback attachment, the initial layout should actually be GENERAL.
					if (!used && attachments[attachment].initialLayout != VK_IMAGE_LAYOUT_UNDEFINED)
						attachments[attachment].initialLayout = current_layout;
				} else {
					if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
						current_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				}

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout) {
					external_input_dependencies |= 1u << subpass;
					external_depth_dependencies |= 1u << subpass;
				}

				depth_stencil_attachment_read |= 1u << subpass;
				input_attachment_read |= 1u << subpass;
				depth->layout = current_layout;
				input->layout = current_layout;
				used = true;
				last_subpass_for_attachment[attachment] = subpass;
			} else if (depth) {
				if (subpassInfos[subpass].depthStencilMode == RenderPassInfo::DepthStencil::READ_WRITE) {
					depth_stencil_attachment_write |= 1u << subpass;
					if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
						current_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				} else {
					if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
						current_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				}

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout)
					external_depth_dependencies |= 1u << subpass;

				depth_stencil_attachment_read |= 1u << subpass;
				depth->layout = current_layout;
				used = true;
				last_subpass_for_attachment[attachment] = subpass;
			} else if (input) {
				if (current_layout != VK_IMAGE_LAYOUT_GENERAL)
					current_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				// If the attachment is first used as an input attachment, the initial layout should actually be
				// SHADER_READ_ONLY_OPTIMAL.
				if (!used && attachments[attachment].initialLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
					attachments[attachment].initialLayout = current_layout;

				// If first subpass changes the layout, we'll need to inject an external subpass dependency.
				if (!used && attachments[attachment].initialLayout != current_layout)
					external_input_dependencies |= 1u << subpass;

				input->layout = current_layout;
				used = true;
				last_subpass_for_attachment[attachment] = subpass;
			} else {
				ASSERTF(true, "Unhandled attachment usage.");
			}
		}

		// If we don't have a specific layout we need to end up in, just
		// use the last one.
		// Assert that we actually use all the attachments we have ...
		ASSERT(!used);
		if (attachments[attachment].finalLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
			ASSERT(current_layout == VK_IMAGE_LAYOUT_UNDEFINED);
			attachments[attachment].finalLayout = current_layout;
		}
	}

	// Only consider preserve masks before last subpass which uses an attachment.
	for (unsigned attachment = 0; attachment < attachmentsCount; attachment++)
		preserve_masks[attachment] &= (1u << last_subpass_for_attachment[attachment]) - 1;

	for (unsigned subpass = 0; subpass < subpassesCount; subpass++) {
		auto& pass = subpasses[subpass];
		unsigned preserve_count = 0;
		for (unsigned attachment = 0; attachment < attachmentsCount; attachment++)
			if (preserve_masks[attachment] & (1u << subpass))
				preserve_count++;

		auto* preserve = preserveAlloc.AllocateInit(preserve_count, 0);
		pass.pPreserveAttachments = preserve;
		pass.preserveAttachmentCount = preserve_count;

		for (unsigned attachment = 0; attachment < attachmentsCount; attachment++)
			if (preserve_masks[attachment] & (1u << subpass))
				*preserve++ = attachment;
	}

	VkRenderPassCreateInfo rp_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	rp_info.subpassCount = subpassesCount;
	rp_info.pSubpasses = subpasses.data();
	rp_info.pAttachments = attachments;
	rp_info.attachmentCount = attachmentsCount;

	// Add external subpass dependencies.
	for (uint32 subpass = 0;  subpass < 32; subpass++){
		if ((1u << subpass) & (external_color_dependencies | external_depth_dependencies | external_input_dependencies)) {
			externalDependencies.emplace_back();
			auto& dep = externalDependencies.back();
			dep.srcSubpass = VK_SUBPASS_EXTERNAL;
			dep.dstSubpass = subpass;

			if (external_bottom_of_pipe_dependencies & (1u << subpass))
				dep.srcStageMask |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

			if (external_color_dependencies & (1u << subpass)) {
				dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dep.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dep.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			}

			if (external_depth_dependencies & (1u << subpass)) {
				dep.srcStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dep.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dep.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dep.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			}

			if (external_input_dependencies & (1u << subpass)) {
				dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
					VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dep.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dep.dstAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
			}
		}
	}

	// Queue up self-dependencies (COLOR | DEPTH) -> INPUT.
	for (uint32 subpass = 0; subpass < 32; subpass++) {
		if ((1u << subpass) & (color_self_dependencies | depth_self_dependencies)) {
			externalDependencies.emplace_back();
			auto& dep = externalDependencies.back();
			dep.srcSubpass = subpass;
			dep.dstSubpass = subpass;
			dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			//if (multiview)
			//	dep.dependencyFlags |= VK_DEPENDENCY_VIEW_LOCAL_BIT_KHR;

			if (color_self_dependencies & (1u << subpass)) {
				dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			}

			if (depth_self_dependencies & (1u << subpass)) {
				dep.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dep.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			dep.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dep.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		}
	}

	// Flush and invalidate caches between each subpass.
	for (unsigned subpass = 1; subpass < subpassesCount; subpass++) {
		externalDependencies.emplace_back();
		auto& dep = externalDependencies.back();
		dep.srcSubpass = subpass - 1;
		dep.dstSubpass = subpass;
		dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		
		//if (multiview)
		//	dep.dependencyFlags |= VK_DEPENDENCY_VIEW_LOCAL_BIT_KHR;

		if (color_attachment_read_write & (1u << (subpass - 1))) {
			dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		if (depth_stencil_attachment_write & (1u << (subpass - 1))) {
			dep.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dep.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		if (color_attachment_read_write & (1u << subpass)) {
			dep.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}

		if (depth_stencil_attachment_read & (1u << subpass)) {
			dep.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dep.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}

		if (depth_stencil_attachment_write & (1u << subpass)) {
			dep.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dep.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}

		if (input_attachment_read & (1u << subpass)) {
			dep.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dep.dstAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		}
	}

	if (!externalDependencies.empty()) {
		rp_info.dependencyCount = (uint32)externalDependencies.size();
		rp_info.pDependencies = externalDependencies.data();
	}

	vkCreateRenderPass(device.GetDevice(), &rp_info, NULL, &renderPass);
}

void Renderer::RenderPass::SetupRenderPass(const VkRenderPassCreateInfo& createInfo)
{
	auto* attachments = createInfo.pAttachments;

	// Store the important subpass information for later.
	for (uint32_t i = 0; i < createInfo.subpassCount; i++) {
		auto& subpass = createInfo.pSubpasses[i];

		SubpassInfo subpass_info = {};
		subpass_info.colorAttachmentsCount = subpass.colorAttachmentCount;
		subpass_info.inputAttachmentsCount = subpass.inputAttachmentCount;
		subpass_info.depthStencilAttachment = *subpass.pDepthStencilAttachment;
		memcpy(subpass_info.colorAttachments, subpass.pColorAttachments,
			subpass.colorAttachmentCount * sizeof(*subpass.pColorAttachments));
		memcpy(subpass_info.inputAttachments, subpass.pInputAttachments,
			subpass.inputAttachmentCount * sizeof(*subpass.pInputAttachments));

		unsigned samples = 0;
		for (unsigned att = 0; att < subpass_info.colorAttachmentsCount; att++) {
			if (subpass_info.colorAttachments[att].attachment == VK_ATTACHMENT_UNUSED)
				continue;

			unsigned samp = attachments[subpass_info.colorAttachments[att].attachment].samples;
			if (samples && (samp != samples))
				ASSERT(samp != samples);
			samples = samp;
		}

		if (subpass_info.depthStencilAttachment.attachment != VK_ATTACHMENT_UNUSED) {
			unsigned samp = attachments[subpass_info.depthStencilAttachment.attachment].samples;
			if (samples && (samp != samples))
				ASSERT(samp != samples);
			samples = samp;
		}

		ASSERT(samples <= 0);
		subpass_info.samples = samples;
		subpassesInfo.push_back(subpass_info);
	}
}

TRE_NS_END
