#include "AlignedStackAllocator.hpp"
#include <Windows.h>

TRE_NS_START

FORCEINLINE AlignedStackAllocator::AlignedStackAllocator(usize total_size, bool autoInit) : m_TotalSize(total_size), m_Offset(0), m_Start(NULL), m_Marker(0)
{
	if (autoInit) {
		this->InternalInit();
	}
};

FORCEINLINE AlignedStackAllocator::~AlignedStackAllocator()
{
	this->Free();
}

FORCEINLINE AlignedStackAllocator& AlignedStackAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = operator new (m_TotalSize);
	return *this;
}

FORCEINLINE void AlignedStackAllocator::InternalInit()
{
	if (m_Start != NULL) return;
	m_Start = operator new (m_TotalSize);
}

FORCEINLINE AlignedStackAllocator& AlignedStackAllocator::Reset()
{
	m_Offset = 0;
	m_Marker = 0;
	return *this;
}

//Alignement must be power of 2!
FORCEINLINE void* AlignedStackAllocator::Allocate(ssize size, usize alignment)
{
	ASSERT(!(alignment >= 1)); ASSERT(!(alignment <= 128)); ASSERT(!((alignment & (alignment - 1)) == 0)); // pwr of 2
	this->InternalInit();
	// Determine total amount of memory to allocate. 
	size_t expandedSize_bytes = size + alignment;
	ASSERTF(!(m_Offset + expandedSize_bytes > m_TotalSize), "Failed to allocate the requested amount of bytes, requested size is bigger than the total size.");
	if (m_Offset + expandedSize_bytes > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	uintptr_t rawAddress = reinterpret_cast<uintptr_t>((char*)m_Start + m_Offset);
	size_t mask = (alignment - 1); 
	uintptr_t misalignment = (rawAddress & mask);
	ptrdiff_t adjustment = alignment - misalignment;
	uintptr_t alignedAddress = rawAddress + adjustment;
	// Store the adjustment in the byte immediately 
	// preceding the adjusted address. 
	ASSERT(!(adjustment < 256)); 
	m_Offset += size + adjustment;
	char* pAlignedMem = reinterpret_cast<char*>(alignedAddress);
	pAlignedMem[-1] = static_cast<char>(adjustment);
	return static_cast<void*>(pAlignedMem);
}

FORCEINLINE void AlignedStackAllocator::Deallocate(void* pMem) {
	const char* pAlignedMem = reinterpret_cast<const char*>(pMem);
	uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(pMem); 
	ptrdiff_t adjustment = static_cast<ptrdiff_t>(pAlignedMem[-1]);
	uintptr_t rawAddress = alignedAddress - adjustment; 
	void* pRawMem = reinterpret_cast<void*>(rawAddress);
	ASSERT(!(usize(m_Start) > usize(pRawMem)));
	m_Offset = (const char*)(pRawMem) - (const char*)m_Start;
}


FORCEINLINE void AlignedStackAllocator::Free()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

FORCEINLINE void AlignedStackAllocator::FreeToMarker()
{
	m_Offset = m_Marker;
}

FORCEINLINE void AlignedStackAllocator::SetCurrentOffsetAsMarker()
{
	m_Marker = m_Offset;
}

FORCEINLINE void AlignedStackAllocator::SetMarker(usize marker)
{
	ASSERTF(!(marker < 0 || marker > m_TotalSize), "Bad usage of AlignedStackAllocator::SetMarker(marker) given marker is out of stach bounds.");
	m_Marker = marker;
}

const usize AlignedStackAllocator::GetMarker() const
{
	return m_Marker;
}

const void* AlignedStackAllocator::GetTop() const
{
	return m_Start;
}

const void* AlignedStackAllocator::GetBottom() const
{
	return (void*)((usize)m_Start + m_Offset);
}

const void AlignedStackAllocator::Dump() const
{
	printf("--------- Stack Dump ---------\n");
	for (usize i = 0; i < m_TotalSize; i++) {
		char* byte = (char*)m_Start + i;
		printf("%d", *byte);
	}
	printf("\n------------------------------\n");
}

TRE_NS_END