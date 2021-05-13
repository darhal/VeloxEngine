#include "LinearAllocator.hpp"

TRE_NS_START

void* LinearAllocator::Allocate(uint32 size, uint32 alignement)
{
	this->InternalInit();

	char* curr_adr = (char*)m_Start + m_Offset;
	const uint32 padding = 0; // CalculatePadding(curr_adr, alignement);
	ASSERTF((m_Offset + size + padding > m_TotalSize), "Failed to allocate the requested amount of bytes, allocator is out of memory.");
	if (m_Offset + size + padding > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	ssize old_off = m_Offset + padding;
	m_Offset += size + padding;
	return (void*)((char*)m_Start + old_off);
}

const void LinearAllocator::Dump() const
{
	printf("--------- Linear Dump ---------\n");
	for (uint32 i = 0; i < m_TotalSize; i++) {
		char* byte = (char*)m_Start + i;
		printf("%d", *byte);
	}
	printf("\n------------------------------\n");
}


TRE_NS_END