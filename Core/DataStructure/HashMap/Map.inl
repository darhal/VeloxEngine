#include "Map.hpp"

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>::Map()
{
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>::Map(Map<K, V, Alloc_t>&& other) : m_RBT(::std::move(other.m_RBT))
{
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>& Map<K, V, Alloc_t>::operator=(Map&& other)
{
	m_RBT.RedBalckTree<K, V, Alloc_t>::~RedBalckTree();
	m_RBT = ::std::move(other.m_RBT);
	return *this;
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>& Map<K, V, Alloc_t>::operator=(const Map& other)
{
	m_RBT.RedBalckTree<K, V, Alloc_t>::~RedBalckTree();
	m_RBT = other.m_RBT;
	return *this;
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>::Map(const Map<K, V, Alloc_t>& other) : m_RBT(other.m_RBT)
{
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE V& Map<K, V, Alloc_t>::Put(const K& key, const V& value)
{
	return m_RBT.Insert(key, value);
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE V* Map<K, V, Alloc_t>::Get(const K& key) const
{
	typename MapTree::RBNode* res = m_RBT.Search(key);
	if (res == NULL)
		return NULL;
	V* vres = &(res->value);
	return vres;
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE V* Map<K, V, Alloc_t>::At(const K & key) const
{
	return this->Get(key);
}

template<typename K, typename V, typename Alloc_t>
template<typename ...Args>
FORCEINLINE V& Map<K, V, Alloc_t>::Emplace(const K& key, Args&&... args)
{
	return m_RBT.Insert(key, ::std::forward<Args>(args)...);
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE V& Map<K, V, Alloc_t>::operator[](const K& key)
{
	V* value = this->Get(key);
	if (value == NULL) {
		return m_RBT.Insert(key);
	}
	return *value;
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE const V& Map<K, V, Alloc_t>::operator[](const K& key) const
{
	V* value = this->Get(key);
	ASSERTF(value == NULL, "Key not found!");
	return *value;
}


template<typename K, typename V, typename Alloc_t>
FORCEINLINE void Map<K, V, Alloc_t>::Remove(const K& key)
{
	m_RBT.Remove(key);
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE bool Map<K, V, Alloc_t>::ContainsKey(const K& key) const
{
	return this->Get(key) != NULL;
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE void Map<K, V, Alloc_t>::Clear()
{
	return m_RBT.Clear();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE bool Map<K, V, Alloc_t>::IsEmpty() const
{
	return m_RBT.IsEmpty();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::Iterator Map<K, V, Alloc_t>::begin() noexcept
{
	return m_RBT.begin();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::Iterator Map<K, V, Alloc_t>::end() noexcept
{
	return m_RBT.end();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::begin() const noexcept
{
	return m_RBT.begin();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::end() const noexcept
{
	return m_RBT.end();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::cbegin() const noexcept
{
	return m_RBT.cbegin();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::cend() const noexcept
{
	return m_RBT.cend();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::Iterator Map<K, V, Alloc_t>::rbegin() noexcept
{
	return m_RBT.rbegin();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::Iterator Map<K, V, Alloc_t>::rend() noexcept
{
	return m_RBT.rend();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::crbegin() const noexcept
{
	return m_RBT.crbegin();
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE typename Map<K, V, Alloc_t>::CIterator Map<K, V, Alloc_t>::crend() const noexcept
{
	return m_RBT.crend();
}
