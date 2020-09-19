#include "Image.hpp"
#include <Renderer/Backend/RenderBackend.hpp>

TRE_NS_START

Renderer::ImageView::ImageView(VkImageView view, const ImageViewCreateInfo& info) : 
	info(info), apiImageView(view)
{
}

Renderer::Image::Image(VkImage image, const ImageCreateInfo& info, const MemoryView& memory) : 
	info(info), imageMemory(memory), apiImage(image)
{
}

Renderer::Image::Image(RenderBackend& renderBackend, VkImage image, VkImageView defaultView, const ImageCreateInfo& createInfo, VkImageViewType viewType) :
	apiImage(image),
	info(createInfo),
	imageMemory{}
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

TRE_NS_END