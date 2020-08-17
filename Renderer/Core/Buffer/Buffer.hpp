#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Internal
	{
		Buffer CreateBuffer(const RenderDevice& renderDevice, VkDeviceSize size, const void* data, uint32 usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, uint32 queueFamilyIndexCount = 0, uint32* queueFamilyIndices = NULL);

		Buffer CreateStagingBuffer(const RenderDevice& renderDevice, VkDeviceSize size, const void* data);

		void DestroyBuffer(const RenderDevice& renderDevice, Buffer& buffer);

		void AllocateMemory(const RenderDevice& renderDevice, Buffer& buffer, VkMemoryPropertyFlags properties);

		uint32 FindMemoryType(const RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);

		void CopyBuffers(VkCommandBuffer cmdBuffer, uint32 count, TransferBufferInfo* transferBufferInfo);

		void EditBuffer(const RenderDevice& renderDevice, Buffer& buffer, VkDeviceSize size, const void* data);
	}
}

TRE_NS_END
