#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class Buffer
	{
	public:
		static Buffer CreateBuffer(const Internal::RenderDevice& renderDevice, VkDeviceSize size, const void* data, uint32 usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, uint32 queueFamilyIndexCount = 0, uint32* queueFamilyIndices = NULL);

		static void DestroyBuffer(const Internal::RenderDevice& renderDevice, Buffer& buffer);

		static uint32 FindMemoryType(const Internal::RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);

		static uint32 FindMemoryTypeIndex(const Internal::RenderDevice& renderDevice, uint32 typeFilter, MemoryUsage usage);

		static void CopyBuffers(VkCommandBuffer cmdBuffer, uint32 count, Internal::TransferBufferInfo* transferBufferInfo);

		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		VkBuffer GetAPIObject() const { return apiBuffer; }
	protected:
		MemoryView		bufferMemory;
		VkBuffer		apiBuffer;

		friend class RenderContext;
	};
}

TRE_NS_END
