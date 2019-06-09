#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

template<typename T>
usize Hash(const T& val)
{
	return std::hash<T>{}(val);
}

template<>
usize Hash(const int32& val)
{
	return val;
}

template<typename K, typename V, usize SIZE = 7>
class HashMap
{
public:
	typedef Pair<K, V> HashNode;
	typedef Vector<HashNode> HashTab_t;

	HashMap();

	FORCEINLINE const HashNode* Put(const K& key, const V& value);
	FORCEINLINE const HashNode* Put(K&& key, V&& value);

	FORCEINLINE V* Get(const K& key);
	FORCEINLINE const V& Get(const K& key) const;
	//FORCEINLINE V& Get(const K& key) const;

private:
	FORCEINLINE usize CalculateIndex(const K& key) const;
	FORCEINLINE HashTab_t* InternalListCheck(usize index);

	static const usize DEFAULT_TABLE_CAPACITY = SIZE;
	static const usize DEFAULT_LIST_CAPACITY = 1;

	//HashTab_t m_HashTable[DEFAULT_TABLE_CAPACITY];
	HashTab_t* m_HashTable;
};

template<typename K, typename V, usize S>
HashMap<K, V, S>::HashMap() 
{
	m_HashTable = Allocate<HashTab_t>(DEFAULT_TABLE_CAPACITY);
	for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		*adr = NULL;
	}
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, S>::HashNode* HashMap<K, V, S>::Put(const K& key, const V& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return  listAdr->EmplaceBack(key, value);
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, S>::HashNode* HashMap<K, V, S>::Put(K&& key, V&& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return  listAdr->EmplaceBack(std::forward<K>(key), std::forward<V>(value));
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, S>::Get(const K& key)
{
	usize index = this->CalculateIndex(key);
	for (HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return &node.second;
		}
	}
	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, S>::Get(const K& key) const
{
	usize index = this->CalculateIndex(key);
	for (const HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return node.second;
		}
	}
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, S>::CalculateIndex(const K& key) const
{
	usize hash = Hash(key);
	return hash % DEFAULT_TABLE_CAPACITY;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S>::HashTab_t* HashMap<K, V, S>::InternalListCheck(usize index)
{
	/*if (m_HashTable == NULL) { 
		m_HashTable = Allocate<HashTab_t>(DEFAULT_TABLE_CAPACITY); 
		for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
			int32* adr = (int32*)(m_HashTable + i);
			*adr = NULL;
		}
	}*/
	HashTab_t* listAdr = m_HashTable + index;
	if (*((int32*)listAdr) == NULL) {
		new (listAdr) HashTab_t(DEFAULT_LIST_CAPACITY);
	}
	return listAdr;
}

TRE_NS_END