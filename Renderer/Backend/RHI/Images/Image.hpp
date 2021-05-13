#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
#include <Renderer/Backend/RHI/MemoryAllocator/MemoryAllocator.hpp>
#include "ImageHelper.hpp"

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

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

        ImageView(RenderDevice& dev, VkImageView view, const ImageViewCreateInfo& info);

		~ImageView();

		FORCEINLINE VkImageView GetApiObject() const { return apiImageView; }

		FORCEINLINE const ImageViewCreateInfo& GetInfo() const { return info; }

		FORCEINLINE const Image* GetImage() const { return info.image; }
	private:
        ImageView() = delete;
	private:
        RenderDevice&       device;
		ImageViewCreateInfo info;
		VkImageView			apiImageView;

        friend class RenderDevice;
	};

	using ImageViewHandle = Handle<ImageView>;

	class Image : public Utils::RefCounterEnabled<Image, ImageDeleter, HandleCounter>
	{
	public:
		friend struct ImageDeleter;

        Image(RenderDevice& device, VkImage image, const ImageCreateInfo& info, const MemoryAllocation& memory);

        Image(RenderDevice& device, VkImage image, VkImageView defaultView, const ImageCreateInfo& createInfo, VkImageViewType viewType);

		~Image();

		void CreateDefaultView(VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D);

		FORCEINLINE VkImageLayout GetLayout(VkImageLayout layout) const { return info.layout == layout ? layout : VK_IMAGE_LAYOUT_GENERAL; }

		FORCEINLINE VkImage GetApiObject() const { return apiImage; }

		FORCEINLINE const ImageCreateInfo& GetInfo() const { return info; }

		FORCEINLINE uint32 GetWidth(uint32 lod = 0) const { return TRE::Math::Max(1u, info.width >> lod); }

		FORCEINLINE uint32 GetHeight(uint32 lod = 0) const { return TRE::Math::Max(1u, info.height >> lod); }

		FORCEINLINE ImageViewHandle GetView() const { return defaultView; }

		FORCEINLINE void SetSwapchainLayout(VkImageLayout layout) { swapchainLayout = layout; };
		
		FORCEINLINE VkImageLayout GetSwapchainLayout() const { return swapchainLayout; }

		FORCEINLINE bool IsSwapchainImage() const { return swapchainLayout != VK_IMAGE_LAYOUT_UNDEFINED; }
	private:
        Image() = delete;
	private:
        RenderDevice&       device;
		ImageCreateInfo info;
        MemoryAllocation imageMemory;
		VkImage apiImage;
		ImageViewHandle defaultView;
		VkImageLayout swapchainLayout;

        friend class RenderDevice;
		friend class StagingManager;
	};

	using ImageHandle = Handle<Image>;
}

TRE_NS_END

