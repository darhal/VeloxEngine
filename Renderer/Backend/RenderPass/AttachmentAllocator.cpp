#include "AttachmentAllocator.hpp"
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::AttachmentAllocator::AttachmentAllocator(RenderDevice& device, bool transient) :
    device(device),
	transient(transient)
{
}

Renderer::ImageView& Renderer::AttachmentAllocator::RequestAttachment(uint32 width, uint32 height, VkFormat format, uint32 index, uint32 samples, uint32 layers)
{
	Hasher h;
	h.u32(width);
	h.u32(height);
	h.u32(format);
	h.u32(index);
	h.u32(samples);
	h.u32(layers);

	auto hash = h.Get();
	auto iv = attachments.Request(hash);

	if (iv != NULL) {
		return *iv->handle->GetView();
	}

	ImageCreateInfo imageInfo;
	if (transient) {
		imageInfo = ImageCreateInfo::TransientRenderTarget(width, height, format);
	} else {
		imageInfo = ImageCreateInfo::RenderTarget(width, height, format);
		imageInfo.layout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	}

	imageInfo.samples = static_cast<VkSampleCountFlagBits>(samples);
	imageInfo.layers = layers;

    auto iv2 = attachments.Emplace(hash, device.CreateImage(imageInfo));
	iv2->handle->CreateDefaultView(GetImageViewType(imageInfo, NULL));
	// printf("Creating image! %llu\n", iv2->GetHash());
	return *iv2->handle->GetView();
}

void Renderer::AttachmentAllocator::Clear()
{
	attachments.Clear();
}

void Renderer::AttachmentAllocator::BeginFrame()
{
	attachments.BeginFrame();
}

void Renderer::AttachmentAllocator::Destroy()
{
    attachments.Clear();
}

TRE_NS_END


