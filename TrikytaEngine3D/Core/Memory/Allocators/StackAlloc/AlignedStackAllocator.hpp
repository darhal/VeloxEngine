#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

class AlignedStackAllocator : BaseAllocator
{
	AlignedStackAllocator(usize total_size, bool autoInit = true);

	~AlignedStackAllocator();

	AlignedStackAllocator& Init();

	AlignedStackAllocator& Reset();

	void Free();

	void* Allocate(ssize size, usize alignment = 2);
	void Deallocate(void* pMem);

	template<typename U, typename... Args>
	U* Allocate(Args&&... arg);

	template<typename T>
	void Deallocate(T* obj);

	void FreeToMarker();
	void SetCurrentOffsetAsMarker();
	void SetMarker(usize marker = 0);
	const usize GetMarker() const;

	const void* GetTop() const;
	const void* GetBottom() const;
	const void Dump() const;

	
private:
	void InternalInit();

	void* m_Start;
	usize m_Offset;
	usize m_TotalSize;
	usize m_Marker;

};

template<typename U, typename... Args>
U* AlignedStackAllocator::Allocate(Args&&... args)
{
	void* curr = this->Allocate(sizeof(U), alignof(U));
	new (curr) U(std::forward<Args>(args)...);
	return curr;
}

template<typename T>
void AlignedStackAllocator::Deallocate(T* obj)
{
	ASSERTF(!(m_Offset <= 0), "Can't roll back anymore. Stack is empty..");
	ASSERTF(!(obj == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0 || obj == NULL) return;
#endif
	obj->T::~T();
	this->Deallocate(obj);
}

TRE_NS_END

