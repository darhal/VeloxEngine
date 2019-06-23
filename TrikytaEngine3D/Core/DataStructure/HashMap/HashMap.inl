#include "HashMap.hpp"

template<typename K, typename V, usize S>
HashMap<K, V, CHAINING, S>::HashMap()
{
	/*m_HashTable = Allocate<HashTab_t>(S);
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		*adr = NULL;
	}*/
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Put(const K& key, const V& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index); 
	return listAdr->EmplaceBack(key, value);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Put(K&& key, V&& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<V>(value));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(const K& key, Args&&... args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(key, std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(K&& key, Args&& ...args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, CHAINING, S>::Get(const K& key)
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
FORCEINLINE const V& HashMap<K, V, CHAINING, S>::Get(const K& key) const
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
FORCEINLINE V& HashMap<K, V, CHAINING, S>::operator[](const K& key)
{
	V* res = this->Get(key);
	if (res != NULL) {
		return *res;
	}
	HashNode* n = this->Put(key);
	return n->second;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, CHAINING, S>::Remove(const K& key)
{
	usize index = this->CalculateIndex(key);
	for (typename HashTab_t::Iterator it = m_HashTable.begin(); it != m_HashTable.end(); it++) {
		m_HashTable->Erease(it);
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::ContainsKey(const K& key) const
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
FORCEINLINE void HashMap<K, V, CHAINING, S>::Clear()
{
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr != NULL) {
			m_HashTable[i].Clear();
		}
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::IsEmpty() const
{
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr != NULL && !m_HashTable[i].IsEmpty()) {
			return false;
		}
	}
	return true;
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, CHAINING, S>::CalculateIndex(const K& key) const
{
	usize hash = Hash(key);
	return hash % S;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashTab_t* HashMap<K, V, CHAINING, S>::InternalListCheck(usize index)
{
	/*if (m_HashTable == NULL) {
		m_HashTable = Allocate<HashTab_t>(S);
		for (usize i = 0; i < S; i++) {
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

// TODO : Implement deleting with tombstone and replacement, implement resize on hitting fresh hold.
// TODO : Bug when there is hash collision

template<typename K, typename V, usize S>
HashMap<K, V, PROBING, S>::HashMap() : m_HashTable(NULL), m_Capacity(S)
{
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Put(const K& key, const V& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(key, value);
	return *listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Put(K&& key, V&& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(std::forward<K>(key), std::forward<V>(value));
	return *listAdr;
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Emplace(const K& key, Args&&... args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(key, std::forward<Args>(args)...);
	return *listAdr;
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Emplace(K&& key, Args&& ...args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(std::forward<K>(key), std::forward<Args>(args)...);
	return *listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, PROBING, S>::Get(const K& key)
{
	uint32 x = 0;
	usize hash = Hash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;

	if (*(reinterpret_cast<int8*>(listAdr)) == NULL) 
		return NULL;

	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;

	// Probing solution since there is a collision here
	while (*(reinterpret_cast<int8*>(listAdr)) != NULL && (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER && tombstoneAdr == NULL) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) {// we finished a cycle and the element doesn't exist.
			return NULL;
		}
	}
	
	if (*(reinterpret_cast<int8*>(listAdr)) != NULL && *(reinterpret_cast<int8*>(listAdr)) != TOMBSTONE_MARKER && (listAdr->first == key)) {
		if (tombstoneAdr != NULL) {	// Seen tombstone already
			CopyRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return &(listAdr->second);
	}

	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, PROBING, S>::Get(const K& key) const
{
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, PROBING, S>::operator[](const K& key)
{
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Remove(const K& key)
{
	uint32 x = 0;
	usize hash = Hash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;

	if (*(reinterpret_cast<int8*>(listAdr)) == NULL)
		return;

	HashTab_t startAdr = listAdr;

	// Probing solution since there is a collision here
	while (*(reinterpret_cast<int8*>(listAdr)) != NULL && (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);

		if (listAdr == startAdr) // we finished a cycle and the element doesn't exist.
			return; 
	}

	if (*(reinterpret_cast<int8*>(listAdr)) != NULL) { // We found the element
		listAdr->~HashNode();
		*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER; // Put Tombstone as its deleted
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::ContainsKey(const K& key) const
{
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Clear()
{
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::IsEmpty() const
{
	return m_HashTable == NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Resize(usize newSize)
{
	/*if (newSize < m_Capacity) return;
	HashTab_t newAdr = Allocate<HashNode>(newSize);
	CopyRangeTo(m_HashTable, newAdr, m_Capacity);
	// Make new slots empty by assigning them to 0 on 8 bits space
	for (usize i = m_Capacity; i < newSize; i++) {
		int8* adr = reinterpret_cast<int8*>(newAdr + i);
		*adr = NULL;
	}
	Free(m_HashTable);
	m_Capacity = newSize;
	m_HashTable = newAdr;*/
	if (newSize < m_Capacity) 
		return;

	HashTab_t newAdr = Allocate<HashNode>(newSize);

	for (usize i = 0; i < newSize; i++) {
		*reinterpret_cast<int8*>(newAdr + i) = NULL;
	}

	usize oldCapacity = m_Capacity;
	m_Capacity = newSize;

	for (usize i = 0; i < oldCapacity; i++) {
		HashTab_t srcAdr = m_HashTable + i;
		if (*(reinterpret_cast<int8*>(srcAdr)) != NULL) {
			this->Reinsert(srcAdr, newAdr);
		}
	}

	Free(m_HashTable);
	m_HashTable = newAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::CalculateAdress(const K& key)
{
	HashTab_t listAdr = this->InternalListCheck(key); // Should handle probing

	// Resize and insert in the right place while there is no place left.
	while (listAdr == NULL) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->InternalListCheck(key);		// Should handle probing
	}

	return listAdr;
}


template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t typename HashMap<K, V, PROBING, S>::Reinsert(HashTab_t src, HashTab_t dest)
{
	uint32 x = 0;
	K& key = (src->first);
	usize hash = Hash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = dest + initialIndex;
	HashTab_t startAdr = listAdr;

	// Probing solution since there is a collision here
	while (*(reinterpret_cast<int8*>(listAdr)) != NULL && (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = dest + this->CalculateIndex(hash, ++x);
		if (listAdr == startAdr) return NULL; // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
	}

	CopyRangeTo(src, listAdr, 1);
	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::InternalListCheck(const K& key)
{
	uint32 x = 0;
	usize hash = Hash(key);
	usize initialIndex = this->CalculateIndex(hash, x);

	if (m_HashTable == NULL) {
		m_HashTable = Allocate<HashNode>(m_Capacity);

		for (usize i = 0; i < m_Capacity; i++) {
			int8* adr = reinterpret_cast<int8*>(m_HashTable + i);
			*adr = NULL;
		}

		HashTab_t listAdr = m_HashTable + initialIndex; // Its safe since the table is empty
		return listAdr;
	}

	HashTab_t listAdr = m_HashTable + initialIndex;
	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;

	// Probing solution since there is a collision here
	while (*(reinterpret_cast<int8*>(listAdr)) != NULL && (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (*(reinterpret_cast<int8*>(listAdr)) == TOMBSTONE_MARKER && tombstoneAdr == NULL) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL; 
	}

	bool seenTombstone = (tombstoneAdr != NULL);

	if (*(reinterpret_cast<int8*>(listAdr)) != NULL) { // Means directly they have same key!
		//if (listAdr->first == key) { // double check might be completely unnecessary 
		listAdr->~HashNode(); // Call dtor since the same key is here already

		if (seenTombstone) {
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}
		//}
	}else if (seenTombstone) {
		listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
	}

	return listAdr;
}


template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateIndex(const usize hash, const uint32 x) const
{
	return ((hash + Probe(x)) % m_Capacity);
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateHash(const K& key) const
{
	return Hash(key);
}