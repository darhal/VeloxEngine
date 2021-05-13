#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Utils/Utils.hpp>

TRE_NS_START

class LinearGenericAlloc
{
public:
	LinearGenericAlloc(uint32 initial_size, bool autoInit = true);

	FORCEINLINE LinearGenericAlloc(uint32 total_size, uint8* start);

	FORCEINLINE LinearGenericAlloc();

	FORCEINLINE ~LinearGenericAlloc();

	uint32 Allocate(uint32 size, uint32 alignement = 0);

	uint32 Deallocate(uint32 handle, uint32 sz, uint32 alignement = 0);

	void* GetPointer(uint32 handle);

	template<typename T>
	FORCEINLINE uint32 Allocate(uint32 count = 1);

	template<typename T>
	FORCEINLINE uint32 Deallocate(uint32 handle, uint32 count = 1);

	template<typename T>
	FORCEINLINE T* GetPointer(uint32 handle);

	FORCEINLINE uint32 GetSize() const { return m_TotalSize; }

	FORCEINLINE uint32 GetOffset() const { return m_Offset; }

	FORCEINLINE uint32 SetOffset(uint32 offset) { return m_Offset = offset; }
private:
	uint8* m_Start;
	uint32 m_Offset;
	uint32 m_TotalSize;

	void Reallocate(uint32 sz);
};

template<typename T>
uint32 LinearGenericAlloc::Allocate(uint32 count)
{
	return Allocate(sizeof(T) * count, alignof(T));
}

template<typename T>
uint32 LinearGenericAlloc::Deallocate(uint32 handle, uint32 count)
{
	Deallocate(handle, sizeof(T) * count, alignof(T));
}

template<typename T>
T* LinearGenericAlloc::GetPointer(uint32 handle)
{
	return reinterpret_cast<T*>(this->GetPointer(handle));
}

LinearGenericAlloc::LinearGenericAlloc(uint32 total_size, uint8* start) :
	m_Start(start), m_Offset(0), m_TotalSize(total_size)
{
}

LinearGenericAlloc::LinearGenericAlloc() :
	m_Start(NULL), m_Offset(0), m_TotalSize(0)
{
}

LinearGenericAlloc::~LinearGenericAlloc()
{
	delete m_Start;
}

TRE_NS_END