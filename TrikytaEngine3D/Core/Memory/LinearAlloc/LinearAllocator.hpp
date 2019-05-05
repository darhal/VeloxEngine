#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>

TRE_NS_START

class LinearAllocator
{
public:
	LinearAllocator(usize total_size);

	~LinearAllocator();

	LinearAllocator& Init();

	LinearAllocator& Reset();

	void Free();

	void* Allocate(ssize size, usize alignement = 1);
	
	template<typename U, typename... Args>
	U* Construct(Args&&... arg);

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
U* LinearAllocator::Construct(Args&&... args)
{
	U* curr_adr = (U*)((char*)m_Start + m_Offset);
	const usize padding = CalculatePadding<U>(curr_adr);
	if (m_Offset + padding + sizeof(U) > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	curr_adr += padding;
	new (curr_adr) U(std::forward<Args>(args)...);
	m_Offset += padding + sizeof(U);
	return curr_adr;
}

template<typename T>
void LinearAllocator::Destroy(T* obj)
{
	ASSERTF(!(m_Offset <= 0), "Attempt to Destroy an object with an empty linear allocator.");
	ASSERTF(!(obj == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0 || obj == NULL) return;
#endif
	obj->T::~T();
	m_Offset -= sizeof(T);
}

TRE_NS_END