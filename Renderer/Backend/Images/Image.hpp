#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	enum ImageMiscFlagBits
	{
		IMAGE_MISC_GENERATE_MIPS_BIT = 1 << 0,
		IMAGE_MISC_FORCE_ARRAY_BIT = 1 << 1,
		IMAGE_MISC_MUTABLE_SRGB_BIT = 1 << 2,
		IMAGE_MISC_VERIFY_FORMAT_FEATURE_SAMPLED_LINEAR_FILTER_BIT = 1 << 7,
		IMAGE_MISC_LINEAR_IMAGE_IGNORE_DEVICE_LOCAL_BIT = 1 << 8,
		IMAGE_MISC_FORCE_NO_DEDICATED_BIT = 1 << 9
	};

	using ImageMiscFlags = uint32;
	using ImageViewMiscFlags = uint32;

	class Image;

	enum class ImageDomain
	{
		PHYSICAL,
		TRANSIENT,
		LINEAR_HOST_CACHED,
		LINEAR_HOST
	};

	struct ImageViewCreateInfo
	{
		ImageViewCreateInfo() :
			image(NULL), format(VK_FORMAT_UNDEFINED), baseLevel(0), levels(VK_REMAINING_MIP_LEVELS),
			baseLayer(0), layers(VK_REMAINING_ARRAY_LAYERS), view_type(VK_IMAGE_VIEW_TYPE_MAX_ENUM),
			misc(0), swizzle({VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A})
		{}

		Image* image;
		VkFormat format;
		uint32 baseLevel;
		uint32 levels;
		uint32 baseLayer;
		uint32 layers;
		VkImageViewType view_type;
		ImageViewMiscFlags misc;
		VkComponentMapping swizzle;
	};

	struct ImageCreateInfo
	{
		ImageDomain domain			  = ImageDomain::PHYSICAL;
		uint32 width				  = 0;
		uint32 height				  = 0;
		uint32 depth				  = 1;
		uint32 levels				  = 1;
		VkFormat format				  = VK_FORMAT_R8G8B8A8_SRGB;
		VkImageType type			  = VK_IMAGE_TYPE_2D;
		uint32 layers				  = 1;
		VkImageUsageFlags usage		  = 0;
		uint32 queueFamilies		  = QueueFamilyFlag::NONE;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		VkImageCreateFlags flags	  = 0;
		ImageMiscFlags misc			  = 0;
		VkImageLayout initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
		VkComponentMapping swizzle	  = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		// const DeviceAllocation** memory_aliases = nullptr;
		// uint32 num_memory_aliases = 0;

		static ImageCreateInfo Texture2D(uint32 width, uint32 height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB)
		{
			ImageCreateInfo info;
			info.width = width;
			info.height = height;
			info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			info.format = format;
			return info;
		}
	};

	class ImageView
	{
	public:
	private:
		ImageViewCreateInfo info;
	};

	class Image
	{
	public:
		Image(VkImage image, const ImageCreateInfo& info, MemoryView memory);

		VkImage GetAPIObject() const { return apiImage; }

		const ImageCreateInfo& GetInfo() const { return info; }

	private:
		Image() = default;
	private:
		ImageCreateInfo info;
		MemoryView imageMemory;
		VkImage apiImage;

		friend class RenderBackend;
		friend class StagingManager;
	};
}

TRE_NS_END

