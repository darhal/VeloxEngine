#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

class StackAllocator
{
public:
	StackAllocator(usize total_size);

	~StackAllocator();

	StackAllocator& Allocate();

	StackAllocator& Reset();

	template<typename U, typename... Args>
	U* Construct(Args&&... arg);

	void* Adress(ssize size);

	void Deallocate();

	void RollBack();

	template<typename T>
	void Destroy(T* obj);

	const void* GetTop() const;
	const void* GetBottom() const;
	const void Dump() const;
private:
	void* m_Start;
	usize m_Offset;
	usize m_TotalSize;
};


template<typename U, typename... Args>
U* StackAllocator::Construct(Args&&... args)
{
	if (m_Offset + sizeof(U) + sizeof(usize) > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	U* curr = (U*)((char*)m_Start + m_Offset);
	new (curr) U(std::forward<Args>(args)...);
	usize obj_size = sizeof(U);
	m_Offset += obj_size; // update the offset
	*(reinterpret_cast<usize*>((char*)m_Start + m_Offset)) = obj_size; // write the size of the data at the end.
	m_Offset += sizeof(usize); // add the size of our header so later we dont accidently write over it.
	return curr;
}

template<typename T>
void StackAllocator::Destroy(T* obj)
{
	ASSERTF(!(m_Offset <= 0), "Can't roll back anymore. Stack is empty..");
	ASSERTF(!(obj == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0 || obj == NULL) return;
#endif
	obj->T::~T();
	char* curr = ((char*)m_Start + m_Offset); // calc the current position
	usize data_size = *(reinterpret_cast<usize*>(curr - sizeof(usize))); // read the data size stored.
	ASSERTF(!(m_Offset - data_size - sizeof(usize) < 0), "Usage of a StackAllocator with corrputed data.");
	ASSERTF(!(data_size != sizeof(T)), "Error destroying element that isnt in the top of the stack or with corrputed stack data.");
	m_Offset -= data_size + sizeof(usize);
}

TRE_NS_END