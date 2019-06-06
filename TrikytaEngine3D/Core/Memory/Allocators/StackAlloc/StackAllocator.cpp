#include "StackAllocator.hpp"
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

StackAllocator::StackAllocator(usize total_size, bool autoInit) : m_TotalSize(total_size), m_Offset(0), m_Start(NULL), m_Marker(0)
{
	if (autoInit) {
		this->InternalInit();
	}
};

StackAllocator::~StackAllocator()
{
	this->Free();
}

StackAllocator& StackAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = operator new (m_TotalSize);
	return *this;
}

StackAllocator& StackAllocator::Reset()
{
	m_Offset = 0;
	m_Marker = 0;
	return *this;
}

void* StackAllocator::Allocate(ssize size, usize alignment)
{
	this->InternalInit(); // Check wether should we init or not ?

	const usize currentAddress = (usize)m_Start + m_Offset;
	usize padding = CalculatePaddingWithHeader(currentAddress, alignment, sizeof(AllocationHeader));
	ASSERTF(!(m_Offset + padding + size > m_TotalSize), "Failed to allocate the requested amount of bytes, requested size is bigger than the total size.");
	ASSERTF(!(m_Offset + padding + size > m_TotalSize), "Stack allocator is out of memory..");
	if (m_Offset + padding + size > m_TotalSize) return NULL;

	m_Offset += padding;
	const usize nextAddress = currentAddress + padding;
	const usize headerAddress = nextAddress - sizeof(AllocationHeader);
	AllocationHeader allocationHeader{ (char)padding };
	AllocationHeader* headerPtr = (AllocationHeader*)headerAddress;
	headerPtr = &allocationHeader;
	m_Offset += size;
	return (void*)nextAddress;
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
	const usize currentAddress = (usize)ptr;
	const usize headerAddress = currentAddress - sizeof(AllocationHeader);
	const AllocationHeader* allocationHeader{ (AllocationHeader *)headerAddress };
	m_Offset = currentAddress - allocationHeader->padding - (usize)m_Start;
}

void StackAllocator::FreeToMarker()
{
	m_Offset = m_Marker;
}

void StackAllocator::SetCurrentOffsetAsMarker()
{
	m_Marker = m_Offset;
}

void StackAllocator::SetMarker(usize marker)
{
	ASSERTF(!(marker < 0 || marker > m_TotalSize), "Bad usage of StackAllocator::SetMarker(marker) given marker is out of stach bounds.");
	m_Marker = marker;
}

usize StackAllocator::GetMarker() const
{
	return m_Marker;
}

const void* StackAllocator::GetTop() const
{
	return m_Start;
}

const void* StackAllocator::GetBottom() const
{
	return (void*)((usize)m_Start + m_Offset);
}

const void StackAllocator::Dump() const
{
	printf("--------- Stack Dump ---------\n");
	for (usize i = 0; i < m_TotalSize; i++) {
		char* byte = (char*)m_Start + i;
		printf("%d", *byte);
	}
	printf("\n------------------------------\n");
}

void StackAllocator::InternalInit()
{
	if (m_Start != NULL) return;
	m_Start = operator new (m_TotalSize);
}

TRE_NS_END