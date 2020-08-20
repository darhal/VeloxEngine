#include "MemoryAllocator.hpp"

TRE_NS_START

Renderer::MemoryAllocator::MemoryAllocator(VkDevice device, uint32_t memoryTypeIndex) :
	memoryPool{ VK_NULL_HANDLE }, allocatedList{ NULL }
{
	this->Init(device, memoryTypeIndex);
}

Renderer::MemoryAllocator::MemoryAllocator() :
	memoryPool{ VK_NULL_HANDLE }, allocatedList{ NULL }
{
}

void Renderer::MemoryAllocator::Init(VkDevice device, uint32_t memoryTypeIndex)
{
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= TOTAL_MEM_SIZE;
	allocInfo.memoryTypeIndex	= memoryTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, NULL, &memoryPool) != VK_SUCCESS) {
		ASSERTF(true, "failed to allocate vertex buffer memory!");
	}
}

Renderer::MemoryView Renderer::MemoryAllocator::Allocate(DeviceSize size, uint32 alignement)
{
	AllocNode* allocNode = new AllocNode{ MemoryView{ memoryPool, 0, size, 0 } , NULL};

	if (allocatedList) {
		AllocNode* nextNode = allocatedList;
		

		while (nextNode->next) {
			nextNode = nextNode->next;
		}

		DeviceSize offset = nextNode->memoryView.offset + nextNode->memoryView.size;
		uint32 padding	  = (alignement - (offset % alignement)) % alignement;

		allocNode->memoryView.offset  = offset + padding;
		allocNode->memoryView.padding = padding;
		allocNode->next				  = allocatedList;

		//  printf("Padding = %d (SIZE: %llu | Alignement: %llu)\n", padding, size, alignement);
	} else {
		allocatedList = allocNode;
	}

	return allocNode->memoryView;
}

TRE_NS_END