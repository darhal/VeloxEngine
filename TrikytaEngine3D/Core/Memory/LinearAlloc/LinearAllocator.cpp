#include "LinearAllocator.hpp"

TRE_NS_START

LinearAllocator::LinearAllocator(usize total_size) : m_TotalSize(total_size), m_Offset(0), m_Start(NULL)
{

};

LinearAllocator::~LinearAllocator()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

LinearAllocator& LinearAllocator::Allocate()
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

void* LinearAllocator::Adress(ssize size)
{
	if (m_Offset + size > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	ssize old_off = m_Offset;
	m_Offset += size;
	return (void*)((char*)m_Start + old_off);
}

void LinearAllocator::Deallocate()
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


TRE_NS_END