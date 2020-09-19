#pragma once

#include <unordered_map>

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Images/ImageHelper.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderBackend;
	class ImageView;

	class RENDERER_API AttachmentAllocator
	{
	public:
		AttachmentAllocator(RenderBackend& backend, bool transient);

		ImageView& RequestAttachment(uint32 width, uint32 height, VkFormat format, uint32 index = 0, uint32 samples = 1, uint32 layers = 1);
	private:
		RenderBackend& renderBackend;
		std::unordered_map<Hash, ImageHandle> attachments;
		bool transient;
	};
}

TRE_NS_END