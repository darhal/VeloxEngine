#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "StackLinkedList.hpp"
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

class PoolAllocator
{
public:
	PoolAllocator(usize chunk_size, usize chunk_num);

	~PoolAllocator();

	PoolAllocator& Init();

	PoolAllocator& Reset();

	void Free();

	void* Allocate(usize size);

	void Deallocate(void* ptr);

	template<typename U, typename... Args>
	U* Construct(Args&&... arg);

	template<typename T>
	void Destroy(T* obj);
private:
	void* m_Start;
	usize m_ChunkSize;
	usize m_ChunkNum;

	struct Link {};
	using Node = StackLinkedList<Link>::Node;
	StackLinkedList<Link> m_FreeList; // store all free pools
};

template<typename U, typename... Args>
U* PoolAllocator::Construct(Args&&... args)
{
	ASSERTF(!(m_ChunkSize < sizeof(U)), "Pool chunk size is smaller than sizeof(U)."); // Doesnt have enough size per chunk
	U* freePosition = (U*)m_FreeList.Pop();
	ASSERTF(!(freePosition == NULL), "Pool is full (empty free positions) //TODO: Allocate another arena in the future.");
	new (freePosition) U(std::forward<Args>(args)...);
	return freePosition;
}

template<typename T>
void PoolAllocator::Destroy(T* obj)
{
	ASSERTF(!(obj == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (obj == NULL) return;
#endif
	obj->T::~T();
	m_FreeList.Push((Node*)obj);
}

TRE_NS_END