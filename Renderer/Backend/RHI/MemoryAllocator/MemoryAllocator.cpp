#include "MemoryAllocator.hpp"
#include <Renderer/Backend/RHI/RenderDevice/RenderDevice.hpp>

TRE_NS_START

// TypedMemoryAllocator

void Renderer::TypedMemoryAllocator::Init(const RenderDevice& device, uint32 memoryTypeIndex)
{
    if (device.GetMemoryProperties().memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        map = true;
    }else{
        map = false;
    }

    this->device = device.GetDevice();
    this->memoryTypeIndex = memoryTypeIndex;
    auto& alloc = allocators.emplace_back();
    alloc.Create(this->device, memoryTypeIndex, MIN_SIZE, NUM_BLOCKS * MIN_SIZE, map);
}

void Renderer::TypedMemoryAllocator::Destroy()
{
    for (auto& alloc : allocators) {
        if (map)
            vkUnmapMemory(device, alloc.gpuMemory);
        alloc.Destroy(device);
    }
}

Renderer::MemoryAllocation Renderer::TypedMemoryAllocator::Allocate(uint64 size, uint64 alignement)
{
	// I think the final padding is going to be always 0 hence this if this wasn't always the case we will revert back this
	// uint64 worstCasePadding = (~MIN_SIZE + 1)& (alignement - 1);
	uint64 worstCasePadding = 0;
    uint32 i = 0;
    BuddyAllocator::Allocation allocation = { UINT64_MAX, UINT64_MAX };

    while (allocation.offset == UINT64_MAX) {
        if (i >= allocators.size()) {
            uint32 lastMaxSize = allocators.back().maxSize;
            auto& alloc = allocators.emplace_back();
            alloc.Create(device, memoryTypeIndex, MIN_SIZE, lastMaxSize * RESIZE_FACTOR, map);
			printf("[%d] Creating new allocator\n", memoryTypeIndex);
        }

        // printf("[mem type ind: %d] Requesting %d from %d\n", memoryTypeIndex, size + worstCasePadding, i);
        allocation = allocators[i].Allocate(size + worstCasePadding);
        i++;
    }

	/*auto padding22 = (~allocation.offset + 1) & (alignement - 1);
	auto offset22 = allocation.offset + (~allocation.offset + 1) & (alignement - 1);
	printf("[%d] Allocating size: %llu | alignement: %llu | offset: %llu | worst case padding: %llu | Final padding: %llu | Final offset: %llu\n",
		memoryTypeIndex, size, alignement, allocation.offset,
		worstCasePadding, padding22, offset22);*/

    MemoryAllocation alloc;
    alloc.memory     = allocators[i - 1].gpuMemory;
    alloc.offset     = (VkDeviceSize)allocation.offset;
    alloc.padding    = (~alloc.offset + 1) & (alignement - 1);
    alloc.offset     += alloc.padding;
    alloc.size       = (VkDeviceSize)allocation.size;
    alloc.alignment  = alignement;
    alloc.mappedData = allocators[i - 1].mappedData;
    alloc.allocKey   = (i - 1) << MemoryAllocation::INDEX_SHIFT | memoryTypeIndex;

	ASSERTF(alloc.padding != 0, "Padding is just assumed to be always 0 that was unfortuently not the case so revert back");
    return alloc;
}

void Renderer::TypedMemoryAllocator::Free(const MemoryAllocation& allocation)
{
    BuddyAllocator::Allocation alloc = { allocation.offset - allocation.padding, allocation.size };
    allocators[MemoryAllocation::GetIndex(allocation)].Free(alloc);
}



// MemoryAllocator2

Renderer::MemoryAllocator2::MemoryAllocator2(Renderer::RenderDevice& device) : renderDevice(device)
{

}

void Renderer::MemoryAllocator2::Init()
{
    for (uint32 i = 0; i < renderDevice.GetMemoryProperties().memoryTypeCount; i++) {
        allocators[i].Init(renderDevice, i);
    }
}

void Renderer::MemoryAllocator2::Destroy()
{
    for (uint32 i = 0; i < renderDevice.GetMemoryProperties().memoryTypeCount; i++) {
        allocators[i].Destroy();
    }
}

Renderer::MemoryAllocation Renderer::MemoryAllocator2::Allocate(uint32 indexType, uint64 size, uint64 alignement)
{
    return allocators[indexType].Allocate(size, alignement);
}

void Renderer::MemoryAllocator2::Free(const MemoryAllocation& alloc)
{
    allocators[MemoryAllocation::GetTypeIndex(alloc)].Free(alloc);
}



































Renderer::MemoryAllocator::AllocKey Renderer::MemoryAllocator::AllocPool::Allocate(DeviceSize size, DeviceSize alignment)
{
	Chunk* chunk = GetLatestChunk(TOTAL_MEM_SIZE);
	uint32 chunkId = uint32(chunks.size() - 1);
	BindingInfo bindInfo{ UINT64_MAX, size, 0, (uint32)alignment };

	if (FindFreeBinding(*chunk, alignment, bindInfo)) {
		// Everything is done in the find free binding function
	} else {
		// printf("\nCreating new chunk!\n");
		DeviceSize chunkSize = TOTAL_MEM_SIZE * chunks.size() + size + alignment;
		chunk = CreateNewChunk(chunkSize);
		chunkId = uint32(chunks.size() - 1);
		FindFreeBinding(*chunk, alignment, bindInfo);
	}

	uint32 bindingId = (uint32)(chunk->bindingList.size());
	chunk->bindingList.emplace_back(bindInfo);
	return AllocKey(memoryTypeIndex, chunkId, bindingId);
}

void Renderer::MemoryAllocator::AllocPool::FreeBinding(AllocKey key)
{
	auto& bindingList = chunks.at(key.GetChunkIndex()).bindingList;
	bindingList.erase(bindingList.cbegin() + key.GetBindingIndex());
}

void Renderer::MemoryAllocator::AllocPool::FreeChunk(AllocKey key)
{
	chunks.erase(chunks.cbegin() + key.GetChunkIndex());
}

void Renderer::MemoryAllocator::AllocPool::Free(AllocKey key)
{
	// TODO: finish the implmentation 
	if (chunks.size()) {
		if (chunks[key.GetBindingIndex()].bindingList.size() == 1) {
			// this->FreeChunk(key);
			this->FreeBinding(key);
		} else {
			// this->FreeBinding(key);
		}
	}
}

void Renderer::MemoryAllocator::AllocPool::DeallocateChunk()
{
	for (uint32 i = 0; i < chunks.size(); i++) {
        // if (i >= 1) continue;
        Chunk& chunk = chunks[i];

        if (!chunk.memory) {
            continue;
        }

        if (chunk.mappedData) {
			vkUnmapMemory(renderDevice->device, chunk.memory);
		}

        vkFreeMemory(renderDevice->device, chunk.memory, NULL);
        chunk.memory = VK_NULL_HANDLE;
	}
}

Renderer::MemoryAllocation Renderer::MemoryAllocator::AllocPool::GetMemoryView(AllocKey key)
{
	uint32 chunkIndex = key.GetChunkIndex();
	uint32 bindingIndex = key.GetBindingIndex();

    MemoryAllocation memView;
	memView.memory = chunks[chunkIndex].memory;
	memView.mappedData = chunks[chunkIndex].mappedData;

	memView.size = chunks[chunkIndex].bindingList[bindingIndex].size;
	memView.offset = chunks[chunkIndex].bindingList[bindingIndex].offset;
	memView.padding = chunks[chunkIndex].bindingList[bindingIndex].padding;
    memView.alignment = chunks[chunkIndex].bindingList[bindingIndex].alignment;
    memView.allocKey = key.key;

	return memView;
}

Renderer::MemoryAllocator::Chunk* Renderer::MemoryAllocator::AllocPool::GetLatestChunk(DeviceSize totalSize)
{
	if (chunks.size() == 0) {
		return CreateNewChunk(totalSize);
	} else {
		return &chunks.back();
	}
}

Renderer::MemoryAllocator::Chunk* Renderer::MemoryAllocator::AllocPool::CreateNewChunk(DeviceSize totalSize)
{
	Chunk chunk{ 0 };
	chunk.totalSize = totalSize;

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = totalSize;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	if (vkAllocateMemory(renderDevice->device, &allocInfo, NULL, &chunk.memory) != VK_SUCCESS) {
		ASSERTF(true, "failed to allocate memory!");
	}

	if (renderDevice->memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		vkMapMemory(renderDevice->device, chunk.memory, 0, totalSize, 0, &chunk.mappedData);
	}

    chunks.emplace_back(chunk);
	return &chunks.back();
}

bool Renderer::MemoryAllocator::FindFreeBinding(const Chunk& chunk, DeviceSize alignment, BindingInfo& bindingOut)
{
	if (bindingOut.size > chunk.totalSize) {
		// Create new chunk this one doesn't have enough size
		return false;
	}

	DeviceSize currentBindingOffset = 0;
	DeviceSize padding = (alignment - (currentBindingOffset % alignment)) % alignment;
	DeviceSize sizeWithPadding = bindingOut.size + padding;

	DeviceSize minDifferenceFit = chunk.totalSize;
	uint32 bindingListLimit = (uint32)chunk.bindingList.size();
	DeviceSize nextBindingOffset;

	for (uint32 i = 0; i < bindingListLimit; i++) {
		const BindingInfo& currentBinding		= chunk.bindingList[i];
		bool inLimit = i + 1 < bindingListLimit;
		
		if (inLimit) {
			nextBindingOffset = chunk.bindingList[i + 1].offset;
		} else {
			nextBindingOffset = chunk.totalSize;
		}
		
		currentBindingOffset = currentBinding.offset + currentBinding.size + currentBinding.padding;
		const DeviceSize sizeDifference = nextBindingOffset - currentBindingOffset;

		padding = (alignment - (currentBindingOffset % alignment)) % alignment;
		sizeWithPadding = bindingOut.size + padding;

		// Speical case when we have one buffer allocated we have to say that the rest is free, the difference comparison will be false
		// If we put <= instead of <, the rest of the buffer will be allocated
		if (sizeDifference >= sizeWithPadding && (bindingListLimit == 1 || sizeDifference < minDifferenceFit)) {
			bindingOut.offset = currentBindingOffset + padding;
			bindingOut.padding = padding;
			minDifferenceFit = sizeDifference;
		}

		// printf("[%u - %u]|", currentBindingOffset, nextBindingOffset);
	}

	if (minDifferenceFit == chunk.totalSize && chunk.bindingList.size()) {
		// No free slots found, we have to create new one
		return false;
	}

	if (bindingOut.offset == UINT64_MAX) {
		bindingOut.offset = currentBindingOffset + padding;
		bindingOut.padding = padding;
	}
	
	// printf("Allocated bloc: [%u - %u] (TOTAL SIZE: %d)\n", bindingOut.offset, bindingOut.offset + bindingOut.size, chunk.totalSize);
	return true;
}

Renderer::MemoryAllocator::MemoryAllocator()
{

}

void Renderer::MemoryAllocator::Init(const Internal::RenderDevice& renderDevice)
{
	this->renderDevice = &renderDevice;

	for (uint32 i = 0; i < renderDevice.memoryProperties.memoryTypeCount; i++) {
		allocatedList[i].renderDevice = this->renderDevice;
		allocatedList[i].memoryTypeIndex = i;
	}
}

Renderer::MemoryAllocation Renderer::MemoryAllocator::Allocate(uint32 memoryTypeIndex, DeviceSize size, DeviceSize alignement)
{
	// printf("Allocate : %d from %d\n", size, memoryTypeIndex);
	AllocKey allocKey = allocatedList[memoryTypeIndex].Allocate(size, alignement);
	return allocatedList[memoryTypeIndex].GetMemoryView(allocKey);
}

void Renderer::MemoryAllocator::Free(AllocKey key)
{
    // TODO:
}

Renderer::MemoryAllocation Renderer::MemoryAllocator::GetMemoryViewFromAllocKey(AllocKey key)
{
	return allocatedList[key.GetMemoryTypeIndex()].GetMemoryView(key);
}

void Renderer::MemoryAllocator::Destroy() {
    for (uint32 i = 0; i < renderDevice->memoryProperties.memoryTypeCount; i++) {
        allocatedList[i].DeallocateChunk();
    }
}

TRE_NS_END


