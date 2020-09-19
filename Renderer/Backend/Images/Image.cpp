#include "Image.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

Renderer::ImageView::ImageView(VkImageView view, const ImageViewCreateInfo& info) : 
	info(info), apiImageView(view)
{
}

Renderer::Image::Image(VkImage image, const ImageCreateInfo& info, const MemoryView& memory) : 
	info(info), imageMemory(memory), apiImage(image), swapchainLayout(VK_IMAGE_LAYOUT_UNDEFINED)
{
}

Renderer::Image::Image(RenderBackend& renderBackend, VkImage image, VkImageView defaultView, const ImageCreateInfo& createInfo, VkImageViewType viewType) :
	apiImage(image),
	info(createInfo),
	imageMemory{}, swapchainLayout(VK_IMAGE_LAYOUT_UNDEFINED)
{
	if (defaultView != VK_NULL_HANDLE) {
		ImageViewCreateInfo info;
		info.image = this;
		info.viewType = viewType;
		info.format = createInfo.format;
		info.baseLevel = 0;
		info.levels = createInfo.levels;
		info.baseLayer = 0;
		info.layers = createInfo.layers;

		this->defaultView = ImageViewHandle(renderBackend.objectsPool.imageViews.Allocate(defaultView, info));
	}
}

Renderer::Image::~Image()
{
	// TODO: !
}

void Renderer::Image::CreateDefaultView(RenderBackend& renderBackend, VkImageViewType viewType)
{
	VkImageViewCreateInfo vkViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vkViewInfo.image = apiImage;
	vkViewInfo.format = info.format;
	vkViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	vkViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	vkViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	vkViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	vkViewInfo.subresourceRange.aspectMask = FormatToAspectMask(info.format);
	vkViewInfo.subresourceRange.baseMipLevel = 0;
	vkViewInfo.subresourceRange.baseArrayLayer = 0;
	vkViewInfo.subresourceRange.levelCount = 1;
	vkViewInfo.subresourceRange.layerCount = 1;
	vkViewInfo.viewType = viewType;

	ImageViewCreateInfo imageViewInfo;
	imageViewInfo.image = this;
	imageViewInfo.viewType = viewType;
	imageViewInfo.format = info.format;
	imageViewInfo.baseLevel = 0;
	imageViewInfo.levels = info.levels;
	imageViewInfo.baseLayer = 0;
	imageViewInfo.layers = info.layers;

	VkImageView imageView;
	vkCreateImageView(renderBackend.GetRenderDevice().GetDevice(), &vkViewInfo, NULL, &imageView);
	this->defaultView = ImageViewHandle(renderBackend.objectsPool.imageViews.Allocate(imageView, imageViewInfo));
}

TRE_NS_END