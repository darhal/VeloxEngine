#include "StackAllocator.hpp"
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

StackAllocator::StackAllocator(usize total_size) : m_TotalSize(total_size), m_Offset(0), m_Start(NULL)
{

};

StackAllocator::~StackAllocator()
{
	if (m_Start != NULL)
		delete m_Start;
}

StackAllocator& StackAllocator::Allocate()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = operator new (m_TotalSize);
	return *this;
}

StackAllocator& StackAllocator::Reset()
{
	m_Offset = 0;
	return *this;
}

void* StackAllocator::Adress(ssize size)
{
	if (m_Offset + size + sizeof(usize) > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	usize old_off = m_Offset;
	m_Offset += size; // update the offset
	*(reinterpret_cast<usize*>((char*)m_Start + m_Offset)) = size; // write the size of the data at the end.
	m_Offset += sizeof(usize); // add the size of our header so later we dont accidently write over it.
	return (void*)((char*)m_Start + old_off);
}

void StackAllocator::Deallocate()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

void StackAllocator::RollBack()
{
	ASSERTF(!(m_Offset <= 0), "Can't roll back anymore. Stack is empty..");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0) return;
#endif
	char* curr = ((char*)m_Start + m_Offset); // calc the current position
	usize data_size = *(reinterpret_cast<usize*>((char*)(curr - sizeof(usize)))); // read the data size stored.
	ASSERTF(!((m_Offset - data_size - sizeof(usize)) < 0), "Usage of a StackAllocator with corrputed data.");
	m_Offset -= data_size + sizeof(usize);
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

TRE_NS_END