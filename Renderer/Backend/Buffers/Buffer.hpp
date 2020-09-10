#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	struct BufferInfo
	{
		DeviceSize size      = 0;
		uint32 usage		 = ~0u;
		MemoryUsage domain   = MemoryUsage::GPU_ONLY;
		uint32 queueFamilies = QueueFamilyFlag::NONE;
	};

	typedef BufferInfo BufferCreateInfo;

	class Buffer
	{
	public:
		static uint32 FindMemoryType(const Internal::RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);

		static uint32 FindMemoryTypeIndex(const Internal::RenderDevice& renderDevice, uint32 typeFilter, MemoryUsage usage);

		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		VkBuffer GetAPIObject() const { return apiBuffer; }

		const BufferInfo& GetBufferInfo() const { return bufferInfo; }

	protected:
		MemoryView bufferMemory;
		BufferInfo bufferInfo;
		VkBuffer   apiBuffer;

		friend class RenderBackend;
	};
}

TRE_NS_END
