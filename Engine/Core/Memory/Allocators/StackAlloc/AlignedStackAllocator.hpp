#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

class AlignedStackAllocator : BaseAllocator
{
	FORCEINLINE AlignedStackAllocator(usize total_size, bool autoInit = true);

	FORCEINLINE ~AlignedStackAllocator();

	FORCEINLINE AlignedStackAllocator& Init();

	FORCEINLINE AlignedStackAllocator& Reset();

	FORCEINLINE void Free();

	FORCEINLINE void* Allocate(ssize size, usize alignment = 2);
	FORCEINLINE void Deallocate(void* pMem);

	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... arg);

	template<typename T>
	FORCEINLINE void Deallocate(T* obj);

	FORCEINLINE void FreeToMarker();
	FORCEINLINE void SetCurrentOffsetAsMarker();
	FORCEINLINE void SetMarker(usize marker = 0);
	FORCEINLINE const usize GetMarker() const;

	const void* GetTop() const;
	const void* GetBottom() const;
	const void Dump() const;
private:
	FORCEINLINE void InternalInit();

	char* m_Start;
	usize m_Offset;
	usize m_TotalSize;
	usize m_Marker;

};

template<typename U, typename... Args>
FORCEINLINE U* AlignedStackAllocator::Allocate(Args&&... args)
{
	void* curr = this->Allocate(sizeof(U), alignof(U));
	new (curr) U(std::forward<Args>(args)...);
	return curr;
}

template<typename T>
FORCEINLINE void AlignedStackAllocator::Deallocate(T* obj)
{
	ASSERTF((m_Offset <= 0), "Can't roll back anymore. Stack is empty..");
	ASSERTF((obj == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0 || obj == NULL) return;
#endif
	obj->T::~T();
	this->Deallocate(obj);
}

typedef AlignedStackAllocator AlignStackAlloc;

TRE_NS_END

