#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/Misc/Maths/Utils.hpp>
#include <initializer_list>
#include <iterator>

TRE_NS_START

template<typename T>
class Vector
{
public:
	template<typename PointerType>
	class GIterator;

	typedef GIterator<T> Iterator;
	typedef GIterator<const T> CIterator;
public:
	FORCEINLINE Vector();

	FORCEINLINE Vector(usize sz);
	
	Vector(const std::initializer_list<T>& list);

	template<usize S>
	Vector(T(&arr)[S]);

	Vector(T* data, usize size);

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
	FORCEINLINE void Erease(Iterator itr);
	FORCEINLINE void Clear();
	FORCEINLINE bool PopBack();
	FORCEINLINE bool PopFront();

	FORCEINLINE bool IsEmpty() const;
	FORCEINLINE usize Capacity() const;
	FORCEINLINE usize Length() const;
	FORCEINLINE usize Size() const;
	FORCEINLINE T* Back() const;
	FORCEINLINE T* Front() const;

	FORCEINLINE void Append(const Vector<T>& other);
	FORCEINLINE void Append(Vector<T>&& other);

	//FORCEINLINE const T* At(usize i);
	//FORCEINLINE const T* operator[](usize i);

	FORCEINLINE T& At(usize i);
	FORCEINLINE T& operator[](usize i);
	FORCEINLINE const T& At(usize i) const;
	FORCEINLINE const T& operator[](usize i) const;

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const Iterator begin() const noexcept;
	FORCEINLINE const Iterator end() const noexcept;

	FORCEINLINE CIterator cbegin() const noexcept;
	FORCEINLINE CIterator cend() const noexcept;

	FORCEINLINE Vector(const Vector<T>& other);
	FORCEINLINE Vector& operator=(const Vector<T>& other);
	FORCEINLINE Vector(Vector<T>&& other);
	FORCEINLINE Vector& operator=(Vector<T>&& other);

	FORCEINLINE Vector& operator+=(const Vector<T>& other);
	FORCEINLINE Vector& operator+=(Vector<T>&& other);

	FORCEINLINE T* StealPtr();
private:
	FORCEINLINE void Reallocate(usize nCap);

private:
	CONSTEXPR static usize DEFAULT_CAPACITY	 = 6;
	CONSTEXPR static usize DEFAULT_GROW_SIZE = 2;

	T* m_Data;
	usize m_Length;
	usize m_Capacity;

public:

	template<typename DataType>
	class GIterator : public std::iterator<std::random_access_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
	{
	public:
		GIterator() noexcept : m_Current(m_Data) { }
		GIterator(DataType* node) noexcept : m_Current(node) { }

		GIterator(const GIterator<DataType>& rawIterator) = default;
		GIterator& operator=(const GIterator<DataType>& rawIterator) = default;

		GIterator(GIterator<DataType>&& rawIterator) = default;
		GIterator& operator=(GIterator<DataType>&& rawIterator) = default;
		
		~GIterator() {}

		bool operator!=(const GIterator& iterator) { return m_Current != iterator.m_Current; }

		DataType& operator*() { return *m_Current; }
		const DataType& operator*() const { return (*m_Current); }

		DataType* operator->() { return m_Current; }
		const DataType* operator->() const { return m_Current; }

		DataType* getPtr() const { return m_Current; }
		const DataType* getConstPtr() const { return m_Current; }

		GIterator<DataType>& operator+=(const ptrdiff_t& movement) { m_Current += movement; return (*this); }
		GIterator<DataType>& operator-=(const ptrdiff_t& movement) { m_Current -= movement; return (*this); }
		GIterator<DataType>& operator++() { m_Current++; return (*this); }
		GIterator<DataType>& operator--() { m_Current--; return (*this); }
		GIterator<DataType>  operator++(int) { auto temp(*this); ++m_Current; return temp; }
		GIterator<DataType>  operator--(int) { auto temp(*this); --m_Current; return temp; }
		GIterator<DataType>  operator+(const ptrdiff_t& movement) { auto oldPtr = m_Current; m_Current += movement; auto temp(*this); m_Current = oldPtr; return temp; }
		GIterator<DataType>  operator-(const ptrdiff_t& movement) { auto oldPtr = m_Current; m_Current -= movement; auto temp(*this); m_Current = oldPtr; return temp; }
	private:
		DataType* m_Current;
	};
};

#include "Vector.inl"

TRE_NS_END