#include "HashMap.hpp"

template<typename K, typename V, usize S>
HashMap<K, V, S, OPEN_ADR>::HashMap()
{
	/*m_HashTable = Allocate<HashTab_t>(DEFAULT_TABLE_CAPACITY);
	for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		*adr = NULL;
	}*/
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, OPEN_ADR>::HashNode& HashMap<K, V, S, OPEN_ADR>::Put(const K& key, const V& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(key, value);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, OPEN_ADR>::HashNode& HashMap<K, V, S, OPEN_ADR>::Put(K&& key, V&& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<V>(value));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, S, OPEN_ADR>::HashNode& HashMap<K, V, S, OPEN_ADR>::Emplace(const K& key, Args&&... args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(key, std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, S, OPEN_ADR>::HashNode& HashMap<K, V, S, OPEN_ADR>::Emplace(K&& key, Args&& ...args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, S, OPEN_ADR>::Get(const K& key)
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
FORCEINLINE const V& HashMap<K, V, S, OPEN_ADR>::Get(const K& key) const
{
	usize index = this->CalculateIndex(key);
	for (const HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return node.second;
		}
	}
	this->Put();
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, S, OPEN_ADR>::operator[](const K& key)
{
	V* res = this->Get(key);
	if (res != NULL) {
		return *res;
	}
	HashNode* n = this->Put(key);
	return n->second;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, S, OPEN_ADR>::Remove(const K& key)
{
	usize index = this->CalculateIndex(key);
	for (typename HashTab_t::Iterator it = m_HashTable.begin(); it != m_HashTable.end(); it++) {
		m_HashTable->Erease(it);
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, S, OPEN_ADR>::ContainsKey(const K& key) const
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = m_HashTable + index;
	if (*((int32*)listAdr) == NULL) {
		return false;
	}
	for (const HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, S, OPEN_ADR>::Clear()
{
	for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr != NULL) {
			m_HashTable[i].Clear();
		}
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, S, OPEN_ADR>::IsEmpty() const
{
	for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr != NULL && !m_HashTable[i].IsEmpty()) {
			return false;
		}
	}
	return true;
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, S, OPEN_ADR>::CalculateIndex(const K& key) const
{
	usize hash = Hash(key);
	return hash % DEFAULT_TABLE_CAPACITY;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, OPEN_ADR>::HashTab_t* HashMap<K, V, S>::InternalListCheck(usize index)
{
	/*if (m_HashTable == NULL) {
		m_HashTable = Allocate<HashTab_t>(DEFAULT_TABLE_CAPACITY);
		for (usize i = 0; i < DEFAULT_TABLE_CAPACITY; i++) {
			int32* adr = (int32*)(m_HashTable + i);
			*adr = NULL;
		}
	}*/
	HashTab_t* listAdr = m_HashTable + index;
	/*if (*((int32*)listAdr) == NULL) {
		new (listAdr) HashTab_t(DEFAULT_LIST_CAPACITY);
	}*/
	return listAdr;
}

/***********************************************************/
/************************ PROBING **************************/
/***********************************************************/

// TODO:

template<typename K, typename V, usize S>
HashMap<K, V, S, PROBING>::HashMap()
{
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, PROBING>::HashNode& HashMap<K, V, S, PROBING>::Put(const K& key, const V& value)
{
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, PROBING>::HashNode& HashMap<K, V, S, PROBING>::Put(K&& key, V&& value)
{
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, S, PROBING>::HashNode& HashMap<K, V, S, PROBING>::Emplace(const K& key, Args&&... args)
{
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, S, PROBING>::HashNode& HashMap<K, V, S, PROBING>::Emplace(K&& key, Args&& ...args)
{
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, S, PROBING>::Get(const K& key)
{
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, S, PROBING>::Get(const K& key) const
{
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, S, PROBING>::operator[](const K& key)
{
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, S, PROBING>::Remove(const K& key)
{
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, S, PROBING>::ContainsKey(const K& key) const
{
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, S, PROBING>::Clear()
{
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, S, PROBING>::IsEmpty() const
{
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, S, PROBING>::CalculateIndex(const K& key) const
{
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, S, PROBING>::HashTab_t HashMap<K, V, S, PROBING>::InternalListCheck(usize index)
{
}