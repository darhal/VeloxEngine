#include "Image.hpp"

TRE_NS_START

Renderer::Image::Image(VkImage image, const ImageCreateInfo& info, MemoryView memory) : 
	info(info), imageMemory(memory), apiImage(image)
{
}


TRE_NS_END

