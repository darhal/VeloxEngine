#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

class LinearAllocator : BaseAllocator
{
public:
	FORCEINLINE LinearAllocator(usize total_size, bool autoInit = true);

	FORCEINLINE ~LinearAllocator();

	FORCEINLINE LinearAllocator& Init();

	FORCEINLINE LinearAllocator& Reset();

	FORCEINLINE void Free();

	FORCEINLINE void* Allocate(ssize size, usize alignement = 1);
	FORCEINLINE void Deallocate(void* ptr);
	
	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... arg);
	template<typename T>
	FORCEINLINE void Deallocate(T* obj);

	FORCEINLINE const void* GetTop() const;
	FORCEINLINE const void* GetBottom() const;
	const void Dump() const;
private:
	FORCEINLINE void InternalInit();

	void* m_Start;
	usize m_Offset;
	usize m_TotalSize;
};

template<typename U, typename... Args>
FORCEINLINE U* LinearAllocator::Allocate(Args&&... args)
{
	this->InternalInit();

	U* curr_adr = (U*)((char*)m_Start + m_Offset);
	const usize padding = CalculatePadding<U>(curr_adr);
	ASSERTF(!(m_Offset + padding + sizeof(U)), "Failed to allocate the requested amount of bytes, allocator is out of memory.");
	if (m_Offset + padding + sizeof(U) > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}
	curr_adr += padding;
	new (curr_adr) U(std::forward<Args>(args)...);
	m_Offset += padding + sizeof(U);
	return curr_adr;
}

template<typename T>
FORCEINLINE void LinearAllocator::Deallocate(T* obj)
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