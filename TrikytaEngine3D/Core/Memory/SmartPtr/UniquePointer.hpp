#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/RefCounter/RefCounter.hpp>

TRE_NS_START

template<typename T>
class UniquePointer
{
public:
	FORCEINLINE UniquePointer(T* ptr) : m_Ptr(ptr) {};
	FORCEINLINE virtual ~UniquePointer();

	FORCEINLINE T& operator*();
	FORCEINLINE T* operator->();

	UniquePointer(const UniquePointer<T>& other) = delete;
	UniquePointer<T>& operator=(const UniquePointer<T>& other) = delete;
private:
	T* m_Ptr;
};

template<typename T>
FORCEINLINE UniquePointer<T>::~UniquePointer()
{
	delete m_Ptr;
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