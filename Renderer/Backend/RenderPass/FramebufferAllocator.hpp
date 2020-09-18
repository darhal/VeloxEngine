#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <unordered_map>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;
	struct RenderPassInfo;
	class Framebuffer;
	class RenderPass;

	class FramebufferAllocator
	{
	public:
		FramebufferAllocator(RenderDevice* device);

		Framebuffer& RequestFramebuffer(const RenderPass& renderPass, const RenderPassInfo& info);

		void Clear();
	private:
		RenderDevice* renderDevice;
		std::unordered_map<Hash, Framebuffer> framebufferCache;
	};
}

TRE_NS_END