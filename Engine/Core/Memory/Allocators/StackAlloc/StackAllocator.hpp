#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

class StackAllocator : BaseAllocator
{
public:
	FORCEINLINE StackAllocator();

	FORCEINLINE StackAllocator(usize total_size, bool autoInit = true);

	FORCEINLINE ~StackAllocator();

	StackAllocator& Init();

	FORCEINLINE StackAllocator& Reset();

	void Free();

	void* Allocate(usize size, usize alignment = 0) override;

	void Deallocate(void* ptr) override;

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

	FORCEINLINE void SetSize(usize nSize);

	void Resize(usize nSize = 0);
private:
	void InternalInit();

	char* m_Start;
	usize m_Offset;
	usize m_TotalSize;
	usize m_Marker;

	CONSTEXPR static uint32 RESIZE_RATIO = 3;

	struct AllocationHeader {
		char padding;
	};
};

FORCEINLINE StackAllocator::StackAllocator() : m_Start(NULL), m_Offset(0), m_TotalSize(0), m_Marker(0)
{
}

FORCEINLINE StackAllocator::StackAllocator(usize total_size, bool autoInit) : m_Start(NULL), m_Offset(0), m_TotalSize(total_size), m_Marker(0)
{
	if (autoInit) {
		this->InternalInit();
	}
}

FORCEINLINE StackAllocator::~StackAllocator()
{
	this->Free();
}

FORCEINLINE void StackAllocator::SetSize(usize nSize)
{
	if (!m_Start)
		m_TotalSize = nSize;
}

FORCEINLINE StackAllocator& StackAllocator::Reset()
{
	m_Offset = 0;
	m_Marker = 0;
	return *this;
}

FORCEINLINE void StackAllocator::FreeToMarker()
{
	m_Offset = m_Marker;
}

FORCEINLINE void StackAllocator::SetCurrentOffsetAsMarker()
{
	m_Marker = m_Offset;
}

FORCEINLINE void StackAllocator::SetMarker(usize marker)
{
	ASSERTF((marker < 0 || marker > m_TotalSize), "Bad usage of StackAllocator::SetMarker(marker) given marker is out of stach bounds.");
	m_Marker = marker;
}

FORCEINLINE usize StackAllocator::GetMarker() const
{
	return m_Marker;
}

FORCEINLINE const void* StackAllocator::GetTop() const
{
	return m_Start;
}

FORCEINLINE const void* StackAllocator::GetBottom() const
{
	return (void*)((usize)m_Start + m_Offset);
}

template<typename U, typename... Args>
FORCEINLINE U* StackAllocator::Allocate(Args&&... args)
{
	void* adr = this->Allocate(sizeof(U), alignof(U));

	if (adr == NULL) 
		return adr;

	new (adr) U(std::forward<Args>(args)...);
	return adr;
}

template<typename T>
FORCEINLINE void StackAllocator::Deallocate(T* obj)
{
	ASSERTF((obj == NULL), "Can't destroy a null pointer...");
	#if not defined(_DEBUG) || defined(NDEBUG)
		if (obj == NULL) return;
	#endif

	obj->T::~T();
	this->Free(obj);
}

typedef StackAllocator StackAlloc;

TRE_NS_END