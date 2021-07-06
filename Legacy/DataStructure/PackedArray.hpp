#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/DataStructure/Pair.hpp>
#include <iterator>

TRE_NS_START

template<typename T>
class PackedArray
{
public:
	typedef uint32 ID;
	typedef Pair<ID, T> Element;

	template<typename PointerType>
	class GIterator;

	typedef GIterator<Element> Iterator;
	typedef GIterator<const Element> CIterator;
	
	struct Index {
		ID index;
		uint32 next_free;
	};

	CONSTEXPR static uint32 DEFAULT_CAPACITY = 1 << 16;
	CONSTEXPR static uint32 INVALID_INDEX = ID(-1);
public:
	PackedArray(uint32 capacity = DEFAULT_CAPACITY);

	ID Add(const T& object);

	template<typename... Args>
	Pair<ID, T>& Emplace(Args... args);

	void Remove(ID id);

	T* Lookup(ID id) const;

	FORCEINLINE T& Get(ID id) const;

	FORCEINLINE T& operator[](ID id) const;

	FORCEINLINE uint32 Size() const { return m_ObjectCount; }

	FORCEINLINE uint32 Capacity() const { return m_Capacity; }

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const Iterator begin() const noexcept;
	FORCEINLINE const Iterator end() const noexcept;

	FORCEINLINE CIterator cbegin() const noexcept;
	FORCEINLINE CIterator cend() const noexcept;
private:
	Pair<ID, T>* m_Objects;
	uint32 m_Capacity;
	uint32 m_ObjectCount;
	uint32 m_FreelistEnqueue;
	uint32 m_FreelistDequeue;

	FORCEINLINE Index* GetIndexArray() const { return (Index*)((uint8*)m_Objects + sizeof(Pair<ID, T>) * m_Capacity); }

public:
	template<typename DataType>
	class GIterator : public std::iterator<std::random_access_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
	{
	public:
		GIterator() : m_Current(m_Objects) {}
		GIterator(DataType* node) : m_Current(node) {}

		GIterator(const GIterator<DataType>& rawIterator) = default;
		GIterator& operator=(const GIterator<DataType>& rawIterator) = default;

		GIterator(GIterator<DataType>&& rawIterator) = default;
		GIterator& operator=(GIterator<DataType>&& rawIterator) = default;

		~GIterator() {}

		bool operator!=(const GIterator& iterator) { return m_Current != iterator.m_Current; }

		typename DataType::Second& operator*() { return m_Current->second; }
		typename DataType::Second& operator*() const { return m_Current->second; }

		typename DataType::Second* operator->() { return &m_Current->second; }
		typename DataType::Second* operator->() const { return &m_Current->second; }

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


template<typename T>
template<typename... Args>
Pair<typename PackedArray<T>::ID, T>& PackedArray<T>::Emplace(Args... args)
{
	ID id = m_FreelistDequeue;
	Index& in = this->GetIndexArray()[id];
	m_FreelistDequeue = in.next_free;
	in.index = m_ObjectCount++;
	Pair<ID, T>* pair = new (&m_Objects[in.index]) Pair<ID, T>(id, std::forward<Args>(args)...);
	return *pair;
}

template<typename T>
T& PackedArray<T>::Get(ID id) const
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id];

	ASSERTF(in.index == INVALID_INDEX, "Invalid ID supplied to the PackedArray:Get().\n");
	return m_Objects[in.index].second;
}

template<typename T>
T& PackedArray<T>::operator[](ID id) const
{
	return this->Get(id);
}

#include "PackedArray.inl"

TRE_NS_END
