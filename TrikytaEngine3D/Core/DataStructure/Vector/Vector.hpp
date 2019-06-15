#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <initializer_list>

TRE_NS_START

template<typename T>
class Vector
{
public:
	typedef T* Iterator;
	typedef T& RefIterator;
	typedef const T* CIterator;

public:
	FORCEINLINE Vector();
	FORCEINLINE Vector(usize sz);
	Vector(const std::initializer_list<T>& list);
	template<usize S>
	Vector(T(&arr)[S]);

	~Vector();

	FORCEINLINE bool Reserve(usize sz);

	template<typename... Args>
	FORCEINLINE T& EmplaceBack(Args&&... args);
	FORCEINLINE T& PushBack(const T& obj);
	FORCEINLINE T& Insert(usize i, const T& obj);
	template<typename... Args>
	FORCEINLINE T& Emplace(usize i, Args&&... args);
	template<typename... Args>
	FORCEINLINE T& EmplaceFront(Args&&... args);
	FORCEINLINE T& PushFront(const T& obj);
	FORCEINLINE void Erease(usize start, usize end);
	FORCEINLINE void Clear();
	FORCEINLINE bool PopBack();
	FORCEINLINE bool PopFront();

	FORCEINLINE bool IsEmpty() const;
	FORCEINLINE usize Capacity() const;
	FORCEINLINE usize Length() const;
	FORCEINLINE usize Size() const;
	FORCEINLINE T* Back() const;
	FORCEINLINE T* Front() const;

	FORCEINLINE const T* At(usize i);
	FORCEINLINE const T* operator[](usize i);
	FORCEINLINE const T& At(usize i) const;
	FORCEINLINE const T& operator[](usize i) const;

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE Vector(const Vector<T>& other);
	FORCEINLINE Vector& operator=(const Vector<T>& other);
	FORCEINLINE Vector(Vector<T>&& other);
	FORCEINLINE Vector& operator=(Vector<T>&& other);

private:
	FORCEINLINE void Reallocate(usize nCap);

private:
	CONSTEXPR static usize DEFAULT_CAPACITY	 = 6;
	CONSTEXPR static usize DEFAULT_GROW_SIZE = 2;

	T* m_Data;
	usize m_Length;
	usize m_Capacity;
};

#include "Vector.inl"

TRE_NS_END