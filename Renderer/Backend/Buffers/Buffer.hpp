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

		static FORCEINLINE BufferInfo UniformBuffer(DeviceSize size)
		{
			return BufferInfo{ size, BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_ONLY , QueueFamilyFlag::NONE };
		}
	};

	typedef BufferInfo BufferCreateInfo;
	class RenderDevice;

	class Buffer : public NoRefCount
	{
	public:
		static uint32 FindMemoryType(const RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);

		static uint32 FindMemoryTypeIndex(const RenderDevice& renderDevice, uint32 typeFilter, MemoryUsage usage);

		static VkBuffer CreateBuffer(const RenderDevice& dev, const BufferInfo& info);

		static VkDeviceMemory CreateBufferMemory(const RenderDevice& dev, const BufferInfo& info, VkBuffer buffer, 
			VkDeviceSize* alignedSize = NULL, uint32 multiplier = 1);

		Buffer(VkBuffer buffer, const BufferInfo& info, const MemoryView& mem);

		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		FORCEINLINE VkBuffer GetApiObject() const { return apiBuffer; }

		FORCEINLINE VkDeviceAddress GetAddress(VkDevice dev) const
		{
			VkBufferDeviceAddressInfo bufferAdrInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
			bufferAdrInfo.buffer = apiBuffer;
			return vkGetBufferDeviceAddress(dev, &bufferAdrInfo);
		}

		FORCEINLINE const BufferInfo& GetBufferInfo() const { return bufferInfo; }

		FORCEINLINE const MemoryView& GetBufferMemory() const { return bufferMemory; }

	protected:
		BufferInfo bufferInfo;
		MemoryView bufferMemory;
		VkBuffer   apiBuffer;

		friend class RenderBackend;
	};

	using BufferHandle = Handle<Buffer>;
}

TRE_NS_END
