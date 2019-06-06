#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/RefCounter/RefCounter.hpp>

TRE_NS_START

template<typename T>
class UniquePointer
{
public:
	FORCEINLINE UniquePointer(T* ptr) : m_Ptr(ptr) {/*ASSERTF(!(m_Ptr == NULL), "Attempt to use unique pointer with a null pointer!");*/}
	FORCEINLINE virtual ~UniquePointer();
	FORCEINLINE UniquePointer();

	FORCEINLINE T& operator*();
	FORCEINLINE T* operator->();
	FORCEINLINE T* GetPointer();

	UniquePointer(UniquePointer<T>& other);
	UniquePointer<T>& operator=(UniquePointer<T>& other);

	UniquePointer(UniquePointer<T>&& other);
	UniquePointer<T>& operator=(UniquePointer<T>&& other);
private:
	T* m_Ptr;
};

template<typename T>
FORCEINLINE UniquePointer<T>::UniquePointer(UniquePointer<T>& other) : m_Ptr(other.m_Ptr)
{
	other.m_Ptr = NULL;
}

template<typename T>
FORCEINLINE UniquePointer<T>& UniquePointer<T>::operator=(UniquePointer<T>& other)
{
	this->m_Ptr = other.m_Ptr;
	other.m_Ptr = NULL;
	return *this;
}

template<typename T>
FORCEINLINE UniquePointer<T>::UniquePointer(UniquePointer<T>&& other) : m_Ptr(other.m_Ptr)
{
	other.m_Ptr = NULL;
}

template<typename T>
FORCEINLINE UniquePointer<T>& UniquePointer<T>::operator=(UniquePointer<T>&& other)
{
	this->m_Ptr = other.m_Ptr;
	other.m_Ptr = NULL;
	return *this;
}

template<typename T>
FORCEINLINE UniquePointer<T>::~UniquePointer()
{
	if (m_Ptr != NULL)
		delete m_Ptr;
}

template<typename T>
FORCEINLINE UniquePointer<T>::UniquePointer() : m_Ptr(NULL)
{
}

template<typename T>
FORCEINLINE T* UniquePointer<T>::GetPointer()
{
	return m_Ptr;
}

template<typename T>
FORCEINLINE T& UniquePointer<T>::operator*()
{
	return m_Ptr;
}

template<typename T>
FORCEINLINE T* UniquePointer<T>::operator->()
{
	return m_Ptr;
}


TRE_NS_END