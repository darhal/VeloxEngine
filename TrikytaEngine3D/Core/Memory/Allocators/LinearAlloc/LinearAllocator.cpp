#include "LinearAllocator.hpp"

TRE_NS_START

LinearAllocator::LinearAllocator(usize total_size, bool autoInit) : m_TotalSize(total_size), m_Offset(0), m_Start(NULL)
{
	if (autoInit) {
		this->InternalInit();
	}
}

LinearAllocator::~LinearAllocator()
{
	this->Free();
}

LinearAllocator& LinearAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = operator new (m_TotalSize);
	return *this;
}

LinearAllocator& LinearAllocator::Reset()
{
	m_Offset = 0;
	return *this;
}

void* LinearAllocator::Allocate(ssize size, usize alignement)
{
	this->InternalInit();

	char* curr_adr = (char*)m_Start + m_Offset;
	const usize padding = CalculatePadding((usize)curr_adr, alignement);
	ASSERTF(!(m_Offset + size + padding > m_TotalSize), "Failed to allocate the requested amount of bytes, allocator is out of memory.");
	if (m_Offset + size + padding > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	ssize old_off = m_Offset + padding;
	m_Offset += size + padding;
	return (void*)((char*)m_Start + old_off);
}

void LinearAllocator::Deallocate(void * ptr)
{

}

void LinearAllocator::Free()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

const void* LinearAllocator::GetTop() const
{
	return m_Start;
}

const void* LinearAllocator::GetBottom() const
{
	return (void*)((usize)m_Start + m_Offset);
}

const void LinearAllocator::Dump() const
{
	printf("--------- Linear Dump ---------\n");
	for (usize i = 0; i < m_TotalSize; i++) {
		char* byte = (char*)m_Start + i;
		printf("%d", *byte);
	}
	printf("\n------------------------------\n");
}

void LinearAllocator::InternalInit()
{
	if (m_Start != NULL) return;
	m_Start = operator new (m_TotalSize);
}


TRE_NS_END