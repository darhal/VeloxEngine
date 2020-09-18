#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "ImageHelper.hpp"

TRE_NS_START

namespace Renderer
{
	class ImageView
	{
	public:
		ImageView(VkImageView view, const ImageViewCreateInfo& info);

		VkImageView GetAPIObject() const { return apiImageView; }

		const ImageViewCreateInfo& GetInfo() const { return info; }

		const Image* GetImage() const { return info.image; }
	private:
		ImageView() = default;
	private:
		ImageViewCreateInfo info;
		VkImageView			apiImageView;

		friend class RenderBackend;
	};

	class Image
	{
	public:
		Image(VkImage image, const ImageCreateInfo& info, const MemoryView& memory);

		VkImage GetAPIObject() const { return apiImage; }

		const ImageCreateInfo& GetInfo() const { return info; }


		uint32 GetWidth(uint32 lod = 0) const { return TRE::Math::Max(1u, info.width >> lod); }

		uint32 GetHeight(uint32 lod = 0) const { return TRE::Math::Max(1u, info.width >> lod); }
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

