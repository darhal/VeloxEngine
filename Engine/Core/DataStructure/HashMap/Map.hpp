#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/BinaryTrees/RedBlackTree.hpp>

TRE_NS_START

template<typename K, typename V, typename Alloc_t = MultiPoolAlloc>
class Map
{
public:
	typedef typename RBT<K, V, Alloc_t>::Iterator Iterator;
	typedef typename RBT<K, V, Alloc_t>::CIterator CIterator;

	Map();

	Map(Map&& other);
	Map(const Map& other);

	FORCEINLINE V& Put(const K& key, const V& value);

	template<typename... Args>
	FORCEINLINE V& Emplace(const K& key, Args&&... args);

	FORCEINLINE V* Get(const K& key) const;

	FORCEINLINE V* At(const K& key) const;

	FORCEINLINE V& operator[](const K& key);

	FORCEINLINE const V& operator[](const K& key) const;

	FORCEINLINE void Remove(const K& key);

	FORCEINLINE bool ContainsKey(const K& key) const;

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE Iterator begin() noexcept;

	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const CIterator begin() const noexcept;

	FORCEINLINE const CIterator end() const noexcept;

	FORCEINLINE const CIterator cbegin() const noexcept;

	FORCEINLINE const CIterator cend() const noexcept;
private:
	RBT<K, V, Alloc_t> m_RBT;
	typedef RBT<K, V, Alloc_t> MapTree;
};

#include "Map.inl"

TRE_NS_END

