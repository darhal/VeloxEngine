#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

class StackAllocator : BaseAllocator
{
public:
	FORCEINLINE StackAllocator(usize total_size, bool autoInit = true);

	FORCEINLINE ~StackAllocator();

	FORCEINLINE StackAllocator& Init();

	FORCEINLINE StackAllocator& Reset();

	FORCEINLINE void Free();

	FORCEINLINE void* Allocate(ssize size, usize alignment);

	FORCEINLINE void Deallocate(void* ptr);

	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... arg);
	template<typename T>
	FORCEINLINE void Deallocate(T* obj);

	FORCEINLINE void FreeToMarker();
	FORCEINLINE void SetCurrentOffsetAsMarker();
	FORCEINLINE void SetMarker(usize marker = 0);
	FORCEINLINE usize GetMarker() const;

	FORCEINLINE const void* GetTop() const;
	FORCEINLINE const void* GetBottom() const;
	const void Dump() const;

	
private:
	FORCEINLINE void InternalInit();

	void* m_Start;
	usize m_Offset;
	usize m_TotalSize;
	usize m_Marker;

	struct AllocationHeader {
		char padding;
	};
};

template<typename U, typename... Args>
FORCEINLINE U* StackAllocator::Allocate(Args&&... args)
{
	void* adr = this->Allocate(sizeof(U), alignof(U));
	if (adr == NULL) return adr;
	new (adr) U(std::forward<Args>(args)...);
	return adr;
}

template<typename T>
FORCEINLINE void StackAllocator::Deallocate(T* obj)
{
	ASSERTF(!(obj == NULL), "Can't destroy a null pointer...");
	#if not defined(_DEBUG) || defined(NDEBUG)
		if (obj == NULL) return;
	#endif

	obj->T::~T();
	this->RollBack(obj);
}

typedef StackAllocator StackAlloc;

TRE_NS_END