#pragma once

#include <unordered_map>

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Images/ImageHelper.hpp>
#include <Renderer/Backend/Images/Image.hpp>
#include <Renderer/Core/Hashmap/TemporaryHashmap.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderBackend;

	class RENDERER_API AttachmentAllocator
	{
	public:
		AttachmentAllocator(RenderBackend& backend, bool transient);

		ImageView& RequestAttachment(uint32 width, uint32 height, VkFormat format, uint32 index = 0, uint32 samples = 1, uint32 layers = 1);

		void Clear();

		void BeginFrame();
	private:
		struct AttachmentNode : Utils::HashmapNode<AttachmentNode>, Utils::ListNode<AttachmentNode>
		{
			explicit AttachmentNode(ImageHandle handle) : handle(std::move(handle)) {};
			
			ImageHandle handle;
		};

		RenderBackend& renderBackend;
		Utils::TemporaryHashmap<AttachmentNode, FRAMEBUFFER_RING_SIZE, false> attachments;
		bool transient;
	};
}

TRE_NS_END