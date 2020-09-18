#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "RenderPass.hpp"

TRE_NS_START

namespace Renderer
{
	class ImageView;
	class RenderDevice;

	class Framebuffer
	{
	public:
		Framebuffer(const RenderDevice& device, const RenderPass& rp, const RenderPassInfo& info);

		VkFramebuffer GetAPIObject() const { return framebuffer; }

		const RenderPass& GetRenderPass() const { return renderPass; }

		uint32 GetWidth() const { return width; }

		uint32 GetHeight() const { return height; }
		
		static uint32 SetupRawViews(VkImageView* views, const RenderPassInfo& info);
		static void ComputeDimensions(const RenderPassInfo& info, uint32& width, uint32& height);
		static void ComputeAttachmentDimensions(const RenderPassInfo& info, uint32 index, uint32& width, uint32& height);
	private:
		RenderPassInfo renderPassInfo;
		VkFramebuffer framebuffer;
		const RenderDevice& renderDevice;
		const RenderPass& renderPass;
		uint32 width;
		uint32 height;
	};
}

TRE_NS_END