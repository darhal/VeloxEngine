#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "RenderPass.hpp"

TRE_NS_START

namespace Renderer
{
	class ImageView;
	class RenderBackend;

	class Framebuffer
	{
	public:
		Framebuffer(const RenderPass& rp, const RenderPassInfo& info);
	private:
		RenderPassInfo renderPassInfo;
		VkFramebuffer framebuffer;
		const RenderPass& renderPass;
		uint32 width;
		uint32 height;
	};
}

TRE_NS_END