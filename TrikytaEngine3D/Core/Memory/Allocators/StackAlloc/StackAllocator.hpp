#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>

TRE_NS_START

class StackAllocator
{
public:
	StackAllocator(usize total_size);

	~StackAllocator();

	StackAllocator& Init();

	StackAllocator& Reset();

	void Free();

	void* Allocate(ssize size, usize alignment);

	void RollBack(void* ptr);

	template<typename U, typename... Args>
	U* Construct(Args&&... arg);

	template<typename T>
	void Destroy(T* obj);

	void FreeToMarker();
	void SetCurrentOffsetAsMarker();
	void SetMarker(usize marker = 0);
	usize GetMarker() const;

	const void* GetTop() const;
	const void* GetBottom() const;
	const void Dump() const;
private:
	void* m_Start;
	usize m_Offset;
	usize m_TotalSize;
	usize m_Marker;

	struct AllocationHeader {
		char padding;
	};
};

template<typename U, typename... Args>
U* StackAllocator::Construct(Args&&... args)
{
	void* adr = this->Allocate(sizeof(U), alignof(U));
	if (adr == NULL) return adr;
	new (adr) U(std::forward<Args>(args)...);
	return adr;
}

template<typename T>
void StackAllocator::Destroy(T* obj)
{
	obj->T::~T();
	this->RollBack(obj);
}

TRE_NS_END