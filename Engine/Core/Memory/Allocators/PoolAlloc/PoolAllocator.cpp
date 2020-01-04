#include "PoolAllocator.hpp"

TRE_NS_START

FORCEINLINE PoolAllocator::PoolAllocator(uint32 chunk_size, uint32 chunk_num, bool autoInit) :
	m_Start(NULL), m_ChunkSize(chunk_size), m_ChunkNum(chunk_num)
{
	if (autoInit) {
		this->InternalInit();
	}
}

FORCEINLINE PoolAllocator::~PoolAllocator()
{
	this->Free();
}

FORCEINLINE PoolAllocator& PoolAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = (char*) operator new (m_ChunkSize*m_ChunkNum);
	this->Reset();
	return *this;
}

FORCEINLINE void PoolAllocator::InternalInit()
{
	if (m_Start != NULL) return;
	m_Start = (char*) operator new (m_ChunkSize*m_ChunkNum);
	this->Reset();
}

FORCEINLINE PoolAllocator& PoolAllocator::Reset()
{
	// Create a linked-list with all free positions
	for (uint32 i = 0; i < m_ChunkNum; i++) {
		ssize address = (ssize)m_Start + i * m_ChunkSize;
		m_FreeList.Push((Node*) address);
	}
	return *this;
}

FORCEINLINE void* PoolAllocator::Allocate(uint32 size, uint32 alignement)
{
	ASSERTF((size > m_ChunkSize), "Failed to allocate the requested amount of bytes, requested size is bigger than block size.");
	if (size > m_ChunkSize) return NULL;
	this->InternalInit();
	Node* freePosition = m_FreeList.Pop();
	ASSERTF((freePosition == NULL), "Pool is full (empty free positions) //TODO: Allocate another arena in the future.");
	return (void*)freePosition;
}

FORCEINLINE void PoolAllocator::Deallocate(void* ptr)
{
	ASSERTF((ptr == NULL), "Can't destroy a null pointer...");
#if not defined(_DEBUG) || defined(NDEBUG)
	if (ptr == NULL) return;
#endif
	m_FreeList.Push((Node*) ptr);
}

FORCEINLINE void PoolAllocator::Free()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

TRE_NS_END