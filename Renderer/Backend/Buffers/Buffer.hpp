#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class RenderDevice;

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
		Buffer(RenderDevice& device, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem);

		void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);

		FORCEINLINE VkBuffer GetApiObject() const { return apiBuffer; }

		FORCEINLINE const BufferInfo& GetBufferInfo() const { return bufferInfo; }

		FORCEINLINE const MemoryView& GetBufferMemory() const { return bufferMemory; }

	protected:
		RenderDevice& device;
		BufferInfo bufferInfo;
		MemoryView bufferMemory;
		VkBuffer   apiBuffer;

		friend class RenderBackend;
		friend class StagingManager;
	};

	using BufferHandle = Handle<Buffer>;
}

TRE_NS_END
