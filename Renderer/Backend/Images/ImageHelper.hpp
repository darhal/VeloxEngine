#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Engine/Core/Misc/Maths/Maths.hpp>

TRE_NS_START

namespace Renderer
{
	using ImageMiscFlags = uint32;
	using ImageViewMiscFlags = uint32;

	class Image;
	class ImageView;

	struct ImageCreateInfo;
	struct ImageViewCreateInfo;

	enum ImageMiscFlagBits
	{
		IMAGE_MISC_GENERATE_MIPS_BIT = 1 << 0,
		IMAGE_MISC_FORCE_ARRAY_BIT = 1 << 1,
		IMAGE_MISC_MUTABLE_SRGB_BIT = 1 << 2,
		IMAGE_MISC_VERIFY_FORMAT_FEATURE_SAMPLED_LINEAR_FILTER_BIT = 1 << 7,
		IMAGE_MISC_LINEAR_IMAGE_IGNORE_DEVICE_LOCAL_BIT = 1 << 8,
		IMAGE_MISC_FORCE_NO_DEDICATED_BIT = 1 << 9
	};

	enum ImageViewMiscFlagBits
	{
		IMAGE_VIEW_MISC_FORCE_ARRAY_BIT = 1 << 0
	};

	enum class ImageDomain
	{
		PHYSICAL,
		TRANSIENT,
		LINEAR_HOST_CACHED,
		LINEAR_HOST
	};

	FORCEINLINE VkImageAspectFlags FormatToAspectMask(VkFormat format)
	{
		switch (format) {
		case VK_FORMAT_UNDEFINED:
			return 0;

		case VK_FORMAT_S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT;

		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;

		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			return VK_IMAGE_ASPECT_DEPTH_BIT;

		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}

	FORCEINLINE uint32 FormatToChannelCount(VkFormat format)
	{
		switch (format) {
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_USCALED:
		case VK_FORMAT_R8G8B8A8_SSCALED:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_B8G8R8A8_SNORM:
		case VK_FORMAT_B8G8R8A8_USCALED:
		case VK_FORMAT_B8G8R8A8_SSCALED:
		case VK_FORMAT_B8G8R8A8_UINT:
		case VK_FORMAT_B8G8R8A8_SINT:
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
			return 4;
		default:
			return 1;
		}
	}

	FORCEINLINE VkPipelineStageFlags ImageUsageToPossibleStages(VkImageUsageFlags usage)
	{
		VkPipelineStageFlags flags = 0;

		if (usage & (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT))
			flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		if (usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
			VkPipelineStageFlags possible = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
				VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
				possible |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			flags &= possible;
		}

		return flags;
	}

	FORCEINLINE VkAccessFlags ImageLayoutToPossibleAccess(VkImageLayout layout)
	{
		switch (layout) {
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			return VK_ACCESS_TRANSFER_READ_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			return VK_ACCESS_TRANSFER_WRITE_BIT;
		default:
			return ~0u;
		}
	}

	FORCEINLINE VkAccessFlags ImageUsageToPossibleAccess(VkImageUsageFlags usage)
	{
		VkAccessFlags flags = 0;

		if (usage & (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT))
			flags |= VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
			flags |= VK_ACCESS_SHADER_READ_BIT;
		if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
			flags |= VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
			flags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		// Transient attachments can only be attachments, and never other resources.
		if (usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
			flags &= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		}

		return flags;
	}

	FORCEINLINE uint32 GetImageMipmapLevels(const VkExtent3D& extent)
	{
		uint32 size = TRE::Math::Max(TRE::Math::Max(extent.width, extent.height), extent.depth);
		uint32 levels = 0;

		while (size) {
			levels++;
			size >>= 1;
		}

		return levels;
	}

	FORCEINLINE VkFormatFeatureFlags ImageUsageToFeatures(VkImageUsageFlags usage)
	{
		VkFormatFeatureFlags flags = 0;
		if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
			flags |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
		if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
			flags |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
		if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			flags |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			flags |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

		return flags;
	}

	bool FORCEINLINE HasStencilComponent(VkFormat format)
	{
		switch (format) {
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
		case VK_FORMAT_S8_UINT:
			return true;

		default:
			return false;
		}
	}

	bool FORCEINLINE HasDepthComponent(VkFormat format)
	{
		switch (format) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;

		default:
			return false;
		}
	}

	bool FORCEINLINE HasDepthOrStencilComponent(VkFormat format)
	{
		return HasDepthComponent(format) || HasStencilComponent(format);
	}


	struct ImageViewCreateInfo
	{
		Image* image = NULL;
		VkFormat format = VK_FORMAT_UNDEFINED;
		uint32 baseLevel = 0;
		uint32 levels = VK_REMAINING_MIP_LEVELS;
		uint32 baseLayer = 0;
		uint32 layers = VK_REMAINING_ARRAY_LAYERS;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		ImageViewMiscFlags misc = 0;
		VkComponentMapping swizzle = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

		static ImageViewCreateInfo ImageView(Image* img, VkImageViewType viewType)
		{
			ImageViewCreateInfo info;
			info.image = img;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			return info;
		}
	};

	struct ImageCreateInfo
	{
		ImageDomain domain = ImageDomain::PHYSICAL;
		uint32 width = 0;
		uint32 height = 0;
		uint32 depth = 1;
		uint32 levels = 1;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		VkImageType type = VK_IMAGE_TYPE_2D;
		uint32 layers = 1;
		VkImageUsageFlags usage = 0;
		uint32 queueFamilies = QueueFamilyFlag::NONE;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		VkImageCreateFlags flags = 0;
		ImageMiscFlags misc = 0;
		VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkComponentMapping swizzle = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		// const DeviceAllocation** memory_aliases = nullptr;
		// uint32 num_memory_aliases = 0;

		static ImageCreateInfo Texture2D(uint32 width, uint32 height, bool mipmap = true,
			VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			ImageCreateInfo info;
			info.width = width;
			info.height = height;
			info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			info.format = format;
			info.layout = layout;

			if (mipmap)
				info.levels = static_cast<uint32>(std::floor(std::log2(TRE::Math::Max(width, height)))) + 1;

			return info;
		}

		static ImageCreateInfo RenderTarget(uint32 width, uint32 height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB)
		{
			ImageCreateInfo info;
			info.width = width;
			info.height = height;
			info.usage = (HasDepthOrStencilComponent(format) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			info.format = format;
			info.layout = HasDepthOrStencilComponent(format) ?
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			return info;
		}

		static ImageCreateInfo TransientRenderTarget(unsigned width, unsigned height, VkFormat format)
		{
			ImageCreateInfo info;
			info.domain = ImageDomain::TRANSIENT;
			info.width = width;
			info.height = height;
			info.format = format;
			info.type = VK_IMAGE_TYPE_2D;
			info.usage = (HasDepthOrStencilComponent(format) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
				VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
			return info;
		}
	};

	FORCEINLINE VkImageViewType GetImageViewType(const ImageCreateInfo& createInfo, const ImageViewCreateInfo* view)
	{
		uint32 layers;
		uint32 base_layer;
		bool force_array;

		if (view) {
			layers = view->layers;
			base_layer = view->baseLayer;
			force_array = view->misc & IMAGE_VIEW_MISC_FORCE_ARRAY_BIT;
		} else {
			layers = createInfo.layers;
			base_layer = 0;
			force_array = createInfo.misc & IMAGE_MISC_FORCE_ARRAY_BIT;
		}

		if (layers == VK_REMAINING_ARRAY_LAYERS)
			layers = createInfo.layers - base_layer;

		switch (createInfo.type) {
		case VK_IMAGE_TYPE_1D:
			ASSERT(createInfo.width < 1);
			ASSERT(createInfo.height != 1);
			ASSERT(createInfo.depth != 1);
			ASSERT(createInfo.samples != VK_SAMPLE_COUNT_1_BIT);

			if (layers > 1 || force_array)
				return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			else
				return VK_IMAGE_VIEW_TYPE_1D;

		case VK_IMAGE_TYPE_2D:
			ASSERT(createInfo.width < 1);
			ASSERT(createInfo.height < 1);
			ASSERT(createInfo.depth != 1);

			if ((createInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) && (layers % 6) == 0) {
				ASSERT(createInfo.width != createInfo.height);

				if (layers > 6 || force_array)
					return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
				else
					return VK_IMAGE_VIEW_TYPE_CUBE;
			} else {
				if (layers > 1 || force_array)
					return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				else
					return VK_IMAGE_VIEW_TYPE_2D;
			}

		case VK_IMAGE_TYPE_3D:
			ASSERT(createInfo.width < 1);
			ASSERT(createInfo.height < 1);
			ASSERT(createInfo.depth < 1);
			return VK_IMAGE_VIEW_TYPE_3D;

		default:
			ASSERTF(true, "BUG");
			return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		}
	}
}


TRE_NS_END