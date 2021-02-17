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
			return BufferInfo{ size, BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_COHERENT , QueueFamilyFlag::NONE };
		}
	};

	typedef BufferInfo BufferCreateInfo;

    struct BufferDeleter
    {
        void operator()(class Buffer* buff);
    };

    class Buffer : public Utils::RefCounterEnabled<Buffer, BufferDeleter, HandleCounter>
	{
	public:
        friend class BufferDeleter;

        Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem);

        Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryView& mem, uint32 unitSize, uint32 ringSize);

        virtual ~Buffer();

        void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0, VkDeviceSize alignement = 0);

        void WriteToRing(VkDeviceSize size, const void* data, VkDeviceSize offset = 0, VkDeviceSize alignement = 0);

		FORCEINLINE VkBuffer GetApiObject() const { return apiBuffer; }

		FORCEINLINE const BufferInfo& GetBufferInfo() const { return bufferInfo; }

		FORCEINLINE const MemoryView& GetBufferMemory() const { return bufferMemory; }

        uint32 GetUnitSize() const { return unitSize; }

        uint32 GetCurrentOffset() const { return bufferIndex * unitSize; }

	protected:
        RenderDevice& device;
        BufferInfo    bufferInfo;
        MemoryView    bufferMemory;
        VkBuffer      apiBuffer;

        uint32		ringSize;
        uint32		unitSize;
        uint32		bufferIndex;

        friend class RenderDevice;
		friend class StagingManager;
	};

	using BufferHandle = Handle<Buffer>;
}

TRE_NS_END
