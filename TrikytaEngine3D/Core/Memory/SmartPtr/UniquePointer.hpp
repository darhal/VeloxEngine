#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/RefCounter/RefCounter.hpp>

TRE_NS_START

template<typename T>
class UniquePointer
{
public:
	UniquePointer(T* ptr);
	virtual ~UniquePointer();

	UniquePointer(const UniquePointer<T>& other);

	FORCEINLINE UniquePointer<T>& operator=(const UniquePointer<T>& other);

	FORCEINLINE T& operator*();
	FORCEINLINE T* operator->();
private:
	RefCounter& m_RefCounter;
	T* m_Ptr;
};

template<typename T>
UniquePointer<T>::UniquePointer(T* ptr) : m_Ptr(ptr)
{
}

template<typename T>
FORCEINLINE UniquePointer<T>& UniquePointer<T>::operator=(const UniquePointer<T>& other)
{
	m_Ptr = other.m_Ptr;
	m_RefCounter = &other.m_RefCounter;
	return *this;
}

template<typename T>
FORCEINLINE T& UniquePointer<T>::operator*()
{
	return *m_Ptr;
}

template<typename T>
FORCEINLINE T* UniquePointer<T>::operator->()
{
	return m_Ptr;
}


TRE_NS_END