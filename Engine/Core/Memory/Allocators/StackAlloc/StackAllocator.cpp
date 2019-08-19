#include "StackAllocator.hpp"
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

StackAllocator& StackAllocator::Init()
{
	if (!m_Start)
		delete m_Start;
		
	m_Start = (char*) operator new (m_TotalSize);
	return *this;
}

void* StackAllocator::Allocate(usize size, usize alignment)
{
	this->InternalInit(); // Check wether should we init or not ?

	const usize currentAddress = (usize)m_Start + m_Offset;
	usize padding = CalculatePaddingWithHeader(currentAddress, alignment, sizeof(AllocationHeader));
	usize new_location = m_Offset + padding + size ;
	// ASSERTF((new_location > m_TotalSize), "Failed to allocate the requested amount of bytes, requested size is bigger than the total size.");
	// ASSERTF((new_location > m_TotalSize), "Stack allocator is out of memory..");
	
	if (new_location > m_TotalSize) {
		this->Resize(padding + size);
	}

	m_Offset += padding;
	const usize nextAddress = currentAddress + padding;
	//const usize headerAddress = nextAddress - sizeof(AllocationHeader);
	//AllocationHeader allocationHeader{ (char)padding };
	//AllocationHeader* headerPtr = (AllocationHeader*)headerAddress;
	//headerPtr = &allocationHeader;
	m_Offset += size;
	return (void*) nextAddress;
}

void StackAllocator::Free()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

void StackAllocator::Deallocate(void* ptr)
{
	ASSERTF((ptr == NULL), "Can't destroy a null pointer...");
	#if not defined(_DEBUG) || defined(NDEBUG)
		if (ptr == NULL) return;
	#endif

	const usize currentAddress = (usize)ptr;
	const usize headerAddress = currentAddress - sizeof(AllocationHeader);
	const AllocationHeader* allocationHeader{ (AllocationHeader *)headerAddress };
	m_Offset = currentAddress - allocationHeader->padding - (usize)m_Start;
}

void StackAllocator::Resize(usize nSize)
{
	usize old_size = m_TotalSize;
	
	if (nSize > m_TotalSize * RESIZE_RATIO){
		m_TotalSize += nSize;
	}else{
		m_TotalSize *= RESIZE_RATIO;
	}
	
	char* new_buffer = (char*) ::operator new (m_TotalSize);
	memcpy(new_buffer, m_Start, old_size);
	delete m_Start;
	m_Start = new_buffer;
}

const void StackAllocator::Dump() const
{
	printf("--------- Stack Dump ---------\n");
	for (usize i = 0; i < m_TotalSize; i++) {
		char* byte = m_Start + i;
		printf("%d", *byte);
	}
	printf("\n------------------------------\n");
}

void StackAllocator::InternalInit()
{
	if (m_Start != NULL) 
		return;

	m_Start = (char*) operator new (m_TotalSize);
}

TRE_NS_END