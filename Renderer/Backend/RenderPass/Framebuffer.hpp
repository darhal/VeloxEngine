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

	private:
		VkFramebuffer framebuffer;
		const RenderPass& renderPass;
		RenderPassInfo renderPassInfo;
		uint32 width;
		uint32 height;
	};
}

TRE_NS_END