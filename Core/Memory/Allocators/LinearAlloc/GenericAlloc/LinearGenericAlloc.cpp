#include "LinearGenericAlloc.hpp"


TRE_NS_START

LinearGenericAlloc::LinearGenericAlloc(uint32 initial_size, bool autoInit) :
	m_Start(NULL), m_Offset(0), m_TotalSize(0)
{ 
	if (autoInit)
		this->Reallocate(initial_size);
}


uint32 LinearGenericAlloc::Allocate(uint32 size, uint32 alignement)
{
	this->Reallocate(size);

	char* curr_adr = (char*)m_Start + m_Offset;
	uint32 old_off = m_Offset + alignement;
	m_Offset += size + alignement;
	return old_off;
}

uint32 LinearGenericAlloc::Deallocate(uint32 handle, uint32 sz, uint32 alignement)
{
	if (handle != m_Offset || sz > m_Offset)
		return 0;

	m_Offset -= sz + alignement;
	return 1;
}

void* LinearGenericAlloc::GetPointer(uint32 handle)
{
	if (handle >= m_TotalSize)
		return NULL;

	return (void*)(m_Start + handle);
}

void LinearGenericAlloc::Reallocate(uint32 sz)
{
	if (sz + m_Offset > m_TotalSize) {
		uint8* new_data = (uint8*) operator new (m_TotalSize * 2 + sz);

		if (m_Start) {
			memcpy(new_data, m_Start, m_Offset * sizeof(uint8));
			delete m_Start;
		}

		m_Start = new_data;
	}
}

TRE_NS_END