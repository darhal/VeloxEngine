#include "Map.hpp"

template<typename K, typename V, typename Alloc_t>
FORCEINLINE Map<K, V, Alloc_t>::Map()
{
}

template<typename K, typename V, typename Alloc_t>
FORCEINLINE void Map<K, V, Alloc_t>::Put(const K& key, const V& value)
{
	m_RBT.Insert(key, value);
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
FORCEINLINE void Map<K, V, Alloc_t>::Emplace(const K& key, Args&&... args)
{
	m_RBT.Insert(key, std::forward<Args>(args)...);
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
