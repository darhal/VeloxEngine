#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>
#include <Legacy/Memory/RefCounter/RefCounter.hpp>

TRE_NS_START

template<typename T, bool IsThreadSafe = false>
class SharedPointer
{
public:
	SharedPointer(T* ptr);
	FORCEINLINE virtual ~SharedPointer();

	FORCEINLINE SharedPointer(const SharedPointer<T, IsThreadSafe>& other);
	FORCEINLINE SharedPointer<T, IsThreadSafe>& operator=(const SharedPointer<T, IsThreadSafe>& other);

	FORCEINLINE T& operator*();
	FORCEINLINE T* operator->();

	FORCEINLINE operator uint32() const;
	FORCEINLINE operator bool() const;
	FORCEINLINE uint32 GetRefCount() const;
private:
	RefCounter<IsThreadSafe>* m_RefCounter;
	T* m_Ptr;
};

template<typename T, bool IsThreadSafe>
SharedPointer<T, IsThreadSafe>::SharedPointer(T* ptr) : m_Ptr(ptr), m_RefCounter(new RefCounter<IsThreadSafe>(1))
{
	ASSERTF((m_Ptr == NULL), "Attempt to use shared pointer with a null pointer!");
}

template<typename T, bool IsThreadSafe>
FORCEINLINE SharedPointer<T, IsThreadSafe>::~SharedPointer()
{
	(*m_RefCounter)--;
	if (m_RefCounter->GetCount() == 0) {
		delete m_RefCounter;
		delete m_Ptr;
		m_Ptr = NULL;
	}
}

template<typename T, bool IsThreadSafe>
FORCEINLINE SharedPointer<T, IsThreadSafe>& SharedPointer<T, IsThreadSafe>::operator=(const SharedPointer<T, IsThreadSafe>& other)
{
	(*m_RefCounter)++;
	m_Ptr = other.m_Ptr;
	m_RefCounter = other.m_RefCounter;
	return *this;
}

template<typename T, bool IsThreadSafe>
FORCEINLINE SharedPointer<T, IsThreadSafe>::SharedPointer(const SharedPointer<T, IsThreadSafe>& other) 
	: m_Ptr(other.m_Ptr), m_RefCounter(other.m_RefCounter)
{
	(*m_RefCounter)++;
}

template<typename T, bool IsThreadSafe>
FORCEINLINE T& SharedPointer<T, IsThreadSafe>::operator*()
{
	return *m_Ptr;
}

template<typename T, bool IsThreadSafe>
FORCEINLINE T* SharedPointer<T, IsThreadSafe>::operator->()
{
	return m_Ptr;
}

template<typename T, bool IsThreadSafe>
FORCEINLINE SharedPointer<T, IsThreadSafe>::operator uint32() const
{
	return m_RefCounter->GetCount();
}

template<typename T, bool IsThreadSafe>
FORCEINLINE uint32 SharedPointer<T, IsThreadSafe>::GetRefCount() const
{
	return m_RefCounter->GetCount();
}

template<typename T, bool IsThreadSafe>
FORCEINLINE SharedPointer<T, IsThreadSafe>::operator bool() const
{
	return m_Ptr != NULL;
}

template<typename T>
using SharedPtr = SharedPointer<T>;

TRE_NS_END