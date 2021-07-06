#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include "StackLinkedList.hpp"
#include <Legacy/Misc/Defines/Debug.hpp>
#include <Legacy/Memory/Allocators/BaseAllocator.hpp>

TRE_NS_START

class PoolAllocator : BaseAllocator
{
public:
	FORCEINLINE PoolAllocator(uint32 chunk_size, uint32 chunk_num, bool autoInit = true);

	FORCEINLINE ~PoolAllocator();

	FORCEINLINE PoolAllocator& Init();

	FORCEINLINE PoolAllocator& Reset();

	FORCEINLINE void Free();

	FORCEINLINE void* Allocate(uint32 size, uint32 alignement = 0);

	FORCEINLINE void Deallocate(void* ptr);

	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... arg);

	template<typename T>
	FORCEINLINE void Deallocate(T* obj);
private:
	FORCEINLINE void InternalInit();

	char* m_Start;
	uint32 m_ChunkSize;
	uint32 m_ChunkNum;

	struct Link {};
	using Node = StackLinkedList<Link>::Node;
	StackLinkedList<Link> m_FreeList; // store all free pools
};

template<typename U, typename... Args>
FORCEINLINE U* PoolAllocator::Allocate(Args&&... args)
{
	ASSERTF((m_ChunkSize < sizeof(U)), "Pool chunk size is smaller than sizeof(U)."); // Doesnt have enough size per chunk
	U* freePosition = (U*)m_FreeList.Pop();
	ASSERTF((freePosition == NULL), "Pool is full (empty free positions) //TODO: Allocate another arena in the future.");
	new (freePosition) U(std::forward<Args>(args)...);
	return freePosition;
}

template<typename T>
FORCEINLINE void PoolAllocator::Deallocate(T* obj)
{
	ASSERTF((obj == NULL), "Can't destroy a null pointer...");
#if !defined(_DEBUG) || defined(NDEBUG)
	if (obj == NULL) return;
#endif
	obj->T::~T();
	m_FreeList.Push((Node*)obj);
}

typedef PoolAllocator PoolAlloc;

TRE_NS_END
