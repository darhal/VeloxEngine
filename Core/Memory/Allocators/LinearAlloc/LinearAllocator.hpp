#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>
#include <utility>

TRE_NS_START

class LinearAllocator : public BaseAllocator
{
public:
	FORCEINLINE LinearAllocator(uint32 total_size, bool autoInit = true);

	FORCEINLINE LinearAllocator(uint32 total_size, uint8* start);

	FORCEINLINE LinearAllocator();

	FORCEINLINE ~LinearAllocator();

	FORCEINLINE void SetSize(uint32 total_size);

	FORCEINLINE LinearAllocator& Init();

	FORCEINLINE LinearAllocator& Reset();

	FORCEINLINE void Free();

	void* Allocate(uint32 size, uint32 alignement = 0) override;

	FORCEINLINE void Deallocate(void* ptr) override;
	
	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... arg);

	template<typename T>
	FORCEINLINE void Deallocate(T* obj);

	FORCEINLINE void SetOffset(uint32 amount);

	FORCEINLINE const void* GetTop() const;
	FORCEINLINE const void* GetBottom() const;

	FORCEINLINE uint32 GetSize() const { return m_TotalSize; }

	FORCEINLINE uint32 GetOffset() const { return m_Offset; }

	const void Dump() const;
private:
	FORCEINLINE void InternalInit();

	char* m_Start;
	uint32 m_Offset;
	uint32 m_TotalSize;
};

FORCEINLINE LinearAllocator::LinearAllocator(uint32 total_size, uint8* start) : m_Start((char*)start), m_Offset(0), m_TotalSize(total_size)
{
}


FORCEINLINE LinearAllocator::LinearAllocator(uint32 total_size, bool autoInit) : m_Start(NULL), m_Offset(0), m_TotalSize(total_size)
{
	if (autoInit) {
		this->InternalInit();
	}
}

FORCEINLINE LinearAllocator::LinearAllocator() : m_Start(NULL), m_Offset(0), m_TotalSize(0)
{
}

FORCEINLINE void LinearAllocator::SetSize(uint32 total_size)
{
	m_TotalSize = total_size;
}

FORCEINLINE LinearAllocator::~LinearAllocator()
{
	this->Free();
}

FORCEINLINE void LinearAllocator::Deallocate(void* ptr)
{
}

FORCEINLINE void LinearAllocator::Free()
{
	if (m_Start != NULL) {
		::operator delete(m_Start);
		m_Start = NULL;
	}
}

FORCEINLINE const void* LinearAllocator::GetTop() const
{
	return (void*) m_Start;
}

FORCEINLINE const void* LinearAllocator::GetBottom() const
{
	return (void*)(m_Start + m_Offset);
}

FORCEINLINE void LinearAllocator::InternalInit()
{
	if (m_Start != NULL) 
		return;
	
	m_Start = (char*) operator new (m_TotalSize);
}

FORCEINLINE LinearAllocator& LinearAllocator::Init()
{
	if (m_Start != NULL)
		delete m_Start;

	m_Start = (char*) operator new (m_TotalSize);
	return *this;
}

void LinearAllocator::SetOffset(uint32 amount)
{
	m_Offset = amount;
}

FORCEINLINE LinearAllocator& LinearAllocator::Reset()
{
	m_Offset = 0;
	return *this;
}

template<typename U, typename... Args>
FORCEINLINE U* LinearAllocator::Allocate(Args&&... args)
{
	this->InternalInit();

	U* curr_adr = (U*)(m_Start + m_Offset);
    const uint32 padding = 0; //CalculatePadding<U>(curr_adr);
	ASSERTF((m_Offset + padding + sizeof(U)), "Failed to allocate the requested amount of bytes, allocator is out of memory.");

	if (m_Offset + padding + sizeof(U) > m_TotalSize) { // Doesnt have enough size
		return NULL;
	}

	curr_adr += padding;
	new (curr_adr) U(::std::forward<Args>(args)...);
	m_Offset += padding + sizeof(U);
	return curr_adr;
}

template<typename T>
FORCEINLINE void LinearAllocator::Deallocate(T* obj)
{
	ASSERTF((m_Offset <= 0), "Attempt to Destroy an object with an empty linear allocator.");
	ASSERTF((obj == NULL), "Can't destroy a null pointer...");
#if !defined(_DEBUG) || defined(NDEBUG)
	if (m_Offset <= 0 || obj == NULL) return;
#endif
	obj->T::~T();
	m_Offset -= sizeof(T);
}

typedef LinearAllocator LinearAlloc;

TRE_NS_END
