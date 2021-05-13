#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
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

		~Framebuffer();

		FORCEINLINE VkFramebuffer GetApiObject() const { return framebuffer; }

		FORCEINLINE const RenderPass& GetRenderPass() const { return renderPass; }

		FORCEINLINE uint32 GetWidth() const { return width; }

		FORCEINLINE uint32 GetHeight() const { return height; }
		
		static uint32 SetupRawViews(VkImageView* views, const RenderPassInfo& info);

		static void ComputeDimensions(const RenderPassInfo& info, uint32& width, uint32& height);
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