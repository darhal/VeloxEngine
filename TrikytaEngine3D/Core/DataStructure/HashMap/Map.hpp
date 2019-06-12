#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/_Infrastructure/BinaryTrees/RedBlackTree.hpp>

TRE_NS_START

template<typename K, typename V, typename Alloc_t = MultiPoolAlloc>
class Map
{
public:
	Map();

	FORCEINLINE void Put(const K& key, const V& value);

	template<typename... Args>
	FORCEINLINE void Emplace(const K& key, Args&&... args);

	FORCEINLINE V* Get(const K& key) const;
private:
	RBT<K, V, Alloc_t> m_RBT;
	typedef RBT<K, V, Alloc_t> MapTree;
};

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
template<typename ...Args>
FORCEINLINE void Map<K, V, Alloc_t>::Emplace(const K& key, Args&&... args)
{
	m_RBT.Insert(key, std::forward<Args>(args)...);
}


TRE_NS_END

