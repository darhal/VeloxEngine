#include "Image.hpp"

TRE_NS_START

Renderer::ImageView::ImageView(VkImageView view, const ImageViewCreateInfo& info) : 
	info(info), apiImageView(view)
{
}

Renderer::Image::Image(VkImage image, const ImageCreateInfo& info, const MemoryView& memory) : 
	info(info), imageMemory(memory), apiImage(image)
{
}

TRE_NS_END