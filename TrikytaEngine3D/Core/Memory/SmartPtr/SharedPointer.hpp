#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/RefCounter/RefCounter.hpp>

TRE_NS_START

template<typename T>
class SharedPointer
{
public:
	SharedPointer(T* ptr);
	FORCEINLINE virtual ~SharedPointer();

	FORCEINLINE SharedPointer(const SharedPointer<T>& other);
	FORCEINLINE SharedPointer<T>& operator=(const SharedPointer<T>& other);

	FORCEINLINE T& operator*();
	FORCEINLINE T* operator->();
private:
	RefCounter* m_RefCounter;
	T* m_Ptr;
};

template<typename T>
SharedPointer<T>::SharedPointer(T* ptr) : m_Ptr(ptr), m_RefCounter(new RefCounter())
{
}

template<typename T>
FORCEINLINE SharedPointer<T>::~SharedPointer()
{
	(*m_RefCounter)--;
	if (m_RefCounter->GetCount() == 0) {
		delete m_RefCounter;
		delete m_Ptr;
	}
}

template<typename T>
FORCEINLINE SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<T>& other)
{
	(*m_RefCounter)++;
	m_Ptr = other.m_Ptr;
	m_RefCounter = other.m_RefCounter;
	return *this;
}

template<typename T>
FORCEINLINE SharedPointer<T>::SharedPointer(const SharedPointer<T>& other) : m_Ptr(other.m_Ptr), m_RefCounter(other.m_RefCounter)
{
	(*m_RefCounter)++;
}

template<typename T>
FORCEINLINE T& SharedPointer<T>::operator*()
{
	return *m_Ptr;
}

template<typename T>
FORCEINLINE T* SharedPointer<T>::operator->()
{
	return m_Ptr;
}


TRE_NS_END