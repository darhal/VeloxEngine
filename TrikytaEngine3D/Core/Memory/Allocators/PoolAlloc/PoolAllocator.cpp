#include "PoolAllocator.hpp"

TRE_NS_START

PoolAllocator::PoolAllocator(usize chunk_size, usize chunk_num, bool autoInit) :
	m_Start(NULL), m_ChunkSize(chunk_size), m_ChunkNum(chunk_num)
{
	if (autoInit) {
		this->InternalInit();
	}
}

PoolAllocator::~PoolAllocator()
{
	this->Free();
}

PoolAllocator& PoolAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;
	m_Start = operator new (m_ChunkSize*m_ChunkNum);
	this->Reset();
	return *this;
}

void PoolAllocator::InternalInit()
{
	if (m_Start != NULL) return;
	m_Start = operator new (m_ChunkSize*m_ChunkNum);
	this->Reset();
}

PoolAllocator& PoolAllocator::Reset()
{
	// Create a linked-list with all free positions
	for (usize i = 0; i < m_ChunkNum; i++) {
		ssize address = (ssize)m_Start + i * m_ChunkSize;
		m_FreeList.Push((Node*) address);
	}
	return *this;
}

void* PoolAllocator::Allocate(usize size, usize alignement)
{
	ASSERTF(!(size > m_ChunkSize), "Failed to allocate the requested amount of bytes, requested size is bigger than block size.");
	if (size > m_ChunkSize) return NULL;
	this->InternalInit();
	Node* freePosition = m_FreeList.Pop();
	ASSERTF(!(freePosition == NULL), "Pool is full (empty free positions) //TODO: Allocate another arena in the future.");
	return (void*)freePosition;
}

void PoolAllocator::Deallocate(void* ptr)
{
	m_FreeList.Push((Node*) ptr);
}

void PoolAllocator::Free()
{
	if (m_Start != NULL) {
		delete m_Start;
		m_Start = NULL;
	}
}

TRE_NS_END