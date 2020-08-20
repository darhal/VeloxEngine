#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	class MemoryAllocator
	{
	public:
		CONSTEXPR static DeviceSize TOTAL_MEM_SIZE = 1024 * 3;

	public:
		MemoryAllocator();

		MemoryAllocator(VkDevice device, uint32_t memoryTypeIndex);

		void Init(VkDevice device, uint32_t memoryTypeIndex);

		MemoryView Allocate(DeviceSize size, uint32 alignement = 0);

	private:
		struct AllocNode
		{
			MemoryView memoryView;
			AllocNode* next;
		};

	private:
		VkDeviceMemory						memoryPool;
		AllocNode*							allocatedList;
	};
}

TRE_NS_END