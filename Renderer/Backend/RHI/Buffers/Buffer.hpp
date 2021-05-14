#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>
#include <Renderer/Backend/RHI/MemoryAllocator/MemoryAllocator.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

	struct BufferInfo
	{
		DeviceSize size      = 0;
		uint32 usage		 = ~0u;
		MemoryDomain domain  = MemoryDomain::GPU_ONLY;
		uint32 queueFamilies = QueueFamilyFlag::NONE;

		static FORCEINLINE BufferInfo UniformBuffer(DeviceSize size)
		{
			return BufferInfo{ size, BufferUsage::UNIFORM_BUFFER, MemoryDomain::CPU_COHERENT , QueueFamilyFlag::NONE };
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
        friend struct BufferDeleter;

        Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryAllocation& mem);

        Buffer(RenderDevice& dev, VkBuffer buffer, const BufferInfo& info, const MemoryAllocation& mem, uint32 unitSize, uint32 ringSize);

        ~Buffer();

        void WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset = 0, VkDeviceSize alignement = 1);

        void WriteToRing(VkDeviceSize size, const void* data, VkDeviceSize offset = 0, VkDeviceSize alignement = 1);

        FORCEINLINE VkBuffer GetApiObject() const { return apiBuffer; }

        FORCEINLINE const BufferInfo& GetBufferInfo() const { return bufferInfo; }

        FORCEINLINE const MemoryAllocation& GetBufferMemory() const { return bufferMemory; }

        uint32 GetUnitSize() const { return unitSize; }

        uint32 GetCurrentOffset() const { return bufferIndex * unitSize; }

        uint32 GetRingSize() const { return ringSize; }

	protected:
        RenderDevice&    device;
        BufferInfo       bufferInfo;
        MemoryAllocation bufferMemory;
        VkBuffer         apiBuffer;

        uint32		ringSize;
        uint32		unitSize;
        uint32		bufferIndex;

        friend class RenderDevice;
		friend class StagingManager;
	};

	using BufferHandle = Handle<Buffer>;
}

TRE_NS_END
