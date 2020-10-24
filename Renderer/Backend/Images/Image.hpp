#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include "ImageHelper.hpp"

TRE_NS_START

namespace Renderer
{
	class RenderBackend;

	struct ImageViewDeleter
	{
		void operator()(ImageView* view);
	};

	struct ImageDeleter
	{
		void operator()(Image* img);
	};

	class ImageView : public Utils::RefCounterEnabled<ImageView, ImageViewDeleter, HandleCounter>
	{
	public:
		friend struct ImageViewDeleter;

		ImageView(RenderBackend& backend, VkImageView view, const ImageViewCreateInfo& info);

		~ImageView();

		FORCEINLINE VkImageView GetAPIObject() const { return apiImageView; }

		FORCEINLINE const ImageViewCreateInfo& GetInfo() const { return info; }

		FORCEINLINE const Image* GetImage() const { return info.image; }
	private:
		ImageView() = default;
	private:
		RenderBackend&	    backend;
		ImageViewCreateInfo info;
		VkImageView			apiImageView;

		friend class RenderBackend;
	};

	using ImageViewHandle = Handle<ImageView>;

	class Image : public Utils::RefCounterEnabled<Image, ImageDeleter, HandleCounter>
	{
	public:
		friend struct ImageDeleter;

		Image(RenderBackend& backend, VkImage image, const ImageCreateInfo& info, const MemoryView& memory);

		Image(RenderBackend& backend, VkImage image, VkImageView defaultView, const ImageCreateInfo& createInfo, VkImageViewType viewType);

		~Image();

		void CreateDefaultView(VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D);

		FORCEINLINE VkImageLayout GetLayout(VkImageLayout layout) const { return info.layout == layout ? layout : VK_IMAGE_LAYOUT_GENERAL; }

		FORCEINLINE VkImage GetAPIObject() const { return apiImage; }

		FORCEINLINE const ImageCreateInfo& GetInfo() const { return info; }

		FORCEINLINE uint32 GetWidth(uint32 lod = 0) const { return TRE::Math::Max(1u, info.width >> lod); }

		FORCEINLINE uint32 GetHeight(uint32 lod = 0) const { return TRE::Math::Max(1u, info.height >> lod); }

		FORCEINLINE ImageViewHandle GetView() const { return defaultView; }

		FORCEINLINE void SetSwapchainLayout(VkImageLayout layout) { swapchainLayout = layout; };
		
		FORCEINLINE VkImageLayout GetSwapchainLayout() const { return swapchainLayout; }

		FORCEINLINE bool IsSwapchainImage() const { return swapchainLayout != VK_IMAGE_LAYOUT_UNDEFINED; }
	private:
		Image() = default;
	private:
		RenderBackend& backend;
		ImageCreateInfo info;
		MemoryView imageMemory;
		VkImage apiImage;
		ImageViewHandle defaultView;
		VkImageLayout swapchainLayout;

		friend class RenderBackend;
		friend class StagingManager;
	};

	using ImageHandle = Handle<Image>;
}

TRE_NS_END

