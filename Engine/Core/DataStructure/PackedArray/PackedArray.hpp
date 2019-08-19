#pragma once

#include <limits>
#include <iterator>
#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/Tuple/Pair.hpp"

TRE_NS_START

template<typename T, usize MAX_OBJECTS = 64 * 1024, typename ID_TYPE = uint32, typename INDEX_TYPE = uint16>
class PackedArray
{
public:
	typedef ID_TYPE ID_t;
	typedef INDEX_TYPE INDEX_t;

    typedef ID_TYPE ID;
    typedef Pair<ID, T> Object; // First the ID then the Object.

	template<typename PointerType>
	class GIterator;

	typedef GIterator<Object> Iterator;
	typedef GIterator<const Object> CIterator;

public:
    CONSTEXPR static INDEX_TYPE MAX = std::numeric_limits<INDEX_TYPE>::max();
    CONSTEXPR static INDEX_TYPE INDEX_MASK = std::numeric_limits<INDEX_TYPE>::max();
    CONSTEXPR static ID_TYPE NEW_OBJECT_ID_ADD = std::numeric_limits<INDEX_TYPE>::max() + 1;

    struct Index {
	    ID id;
	    INDEX_TYPE index;
	    INDEX_TYPE next;
    };

    PackedArray();

	~PackedArray();

	FORCEINLINE INDEX_TYPE CompressID(ID id) const;

	FORCEINLINE bool Has(ID id);
	
	FORCEINLINE Object& Lookup(ID id);
	
	ID Add(const T& obj);

	ID Add(T&& obj);

	template<typename... Args>
	ID Emplace(Args&&... args);
	
	void Remove(ID id);

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const Iterator begin() const noexcept;
	FORCEINLINE const Iterator end() const noexcept;

	FORCEINLINE CIterator cbegin() const noexcept;
	FORCEINLINE CIterator cend() const noexcept;

private:
    Object* m_Objects; // Object m_Objects[MAX_OBJECTS];
 	Index m_Indices[MAX_OBJECTS];

    ID m_NumObjects;
    INDEX_TYPE m_FreelistEnqueue;
	INDEX_TYPE m_FreelistDequeue;

public:

	template<typename DataType>
	class GIterator : public std::iterator<std::random_access_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
	{
	public:
		GIterator() : m_Current(m_Objects) { }
		GIterator(DataType* node) : m_Current(node) { }

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

#include "PackedArray.inl"

TRE_NS_END
