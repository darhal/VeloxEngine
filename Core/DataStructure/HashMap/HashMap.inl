#include "HashMap.hpp"

TRE_NS_START

template<typename K, typename V, usize S>
HashMap<K, V, CHAINING, S>::HashMap()
{
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
	return listAdr->EmplaceBack(::std::forward<K>(key), ::std::forward<V>(value));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(const K& key, Args&&... args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(key, ::std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(K&& key, Args&& ...args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(::std::forward<K>(key), ::std::forward<Args>(args)...);
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
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, CHAINING, S>::operator[](const K& key)
{
	V* res = this->Get(key);
	if (res) {
		return *res;
	}
	HashNode& n = this->Put(key, V());
	return n.second;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, CHAINING, S>::Remove(const K& key)
{
	usize index = this->CalculateIndex(key);
	HashTab_t& tab = m_HashTable[index];
	for (typename HashTab_t::Iterator it = tab.begin(); it != tab.end(); it++) {
		m_HashTable->Erease(it);
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::ContainsKey(const K& key) const
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = m_HashTable + index;
	if (!*((int32*)listAdr)) {
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
		if (!*adr) {
			m_HashTable[i].Clear();
		}
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::IsEmpty() const
{
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr && !m_HashTable[i].IsEmpty()) {
			return false;
		}
	}
	return true;
}

template<typename K, typename V, usize SIZE>
FORCEINLINE usize HashMap<K, V, PROBING, SIZE>::GetSize() const
{
	return m_UsedBuckets;
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, CHAINING, S>::CalculateIndex(const K& key) const
{
	usize hash = Hash::GetHash(key);
	return hash % S;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashTab_t* HashMap<K, V, CHAINING, S>::InternalListCheck(usize index)
{
	/*if (!m_HashTable) {
		m_HashTable = Allocate<HashTab_t>(S);
		for (usize i = 0; i < S; i++) {
			int32* adr = (int32*)(m_HashTable + i);
			*adr = NULL;
		}
	}*/
	HashTab_t* listAdr = m_HashTable + index;
	/*if (!*((int32*)listAdr)) {
		new (listAdr) HashTab_t(DEFAULT_LIST_CAPACITY);
	}*/
	return listAdr;
}

/***********************************************************/
/************************ PROBING **************************/
/***********************************************************/

// TODO : This memory leak must be fixed! dtor is doing nothing apparently.

template<typename K, typename V, usize S>
HashMap<K, V, PROBING, S>::HashMap() : m_HashTable(NULL), m_Capacity(S), m_UsedBuckets(0)
{
}

template<typename K, typename V, usize S>
HashMap<K, V, PROBING, S>::~HashMap()
{
	if (m_HashTable != NULL){
        DestroyMap(*this);
	    ::operator delete(m_HashTable);
		m_HashTable = NULL;
	}
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashPair& HashMap<K, V, PROBING, S>::Put(const K& key, const V& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	return *(new (&listAdr->pair) HashPair(key, value));
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashPair& HashMap<K, V, PROBING, S>::Put(K&& key, V&& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	return *(new (&listAdr->pair) HashPair(::std::forward<K>(key), ::std::forward<V>(value)));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashPair& HashMap<K, V, PROBING, S>::Emplace(const K& key, Args&&... args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	return *(new (&listAdr->pair) HashPair(key, ::std::forward<Args>(args)...));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashPair& HashMap<K, V, PROBING, S>::Emplace(K&& key, Args&& ...args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	return *(new (&listAdr->pair) HashPair(::std::forward<K>(key), ::std::forward<Args>(args)...));
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, PROBING, S>::GetKeyPtr(const K& key) const
{
	if (m_HashTable == NULL)
		return NULL;

	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;
	uint8 elementMarker = listAdr->tag;

	if (elementMarker == HashNode::FREE)
		return NULL;

	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	
	// Probing solution since there is a collision here
	while (elementMarker != HashNode::FREE && (elementMarker == HashNode::TOMBSTONE || !(listAdr->pair.first == key))) {
		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && !tombstoneAdr) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) { // we finished a cycle and the element doesn't exist.
			return NULL;
		}
	}
	
	if (elementMarker != HashNode::FREE && elementMarker != TOMBSTONE_MARKER /*&& listAdr->pair.first == key*/) {

		if (tombstoneAdr) {	// Seen tombstone already
			MoveRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			listAdr->tag = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return &(listAdr->pair.second);
	}

	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, PROBING, S>::Get(const K& key)
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return listAdr->pair.second;
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::HashPair& HashMap<K, V, PROBING, S>::GetPair(const K& key) const
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return listAdr->pair;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashPair* HashMap<K, V, PROBING, S>::GetPairPtr(const K& key) const
{
	if (m_HashTable == NULL)
		return NULL;

	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;
	uint8 elementMarker = listAdr->tag;

	if (elementMarker == HashNode::FREE)
		return NULL;

	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	
	// Probing solution since there is a collision here
	while (elementMarker != HashNode::FREE && (elementMarker == HashNode::TOMBSTONE || !(listAdr->pair.first == key))) {
		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && !tombstoneAdr) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) {// we finished a cycle and the element doesn't exist.
			return NULL;
		}
	}
	
	if (elementMarker == HashNode::OCCUPIED /*&& listAdr->pair.first == key*/) {

		if (tombstoneAdr) {	// Seen tombstone already
			MoveRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			listAdr->tag = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return &listAdr->pair;
	}

	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, PROBING, S>::operator[](const K& key)
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return listAdr->pair.second;
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, PROBING, S>::operator[](const K& key) const
{
	V* ptr = this->GetKeyPtr(key);
	ASSERTF(ptr == NULL, "Attempt to access element at an invalid key.");
	return *ptr;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Remove(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;
	uint8 elementMarker = listAdr->tag;
	HashTab_t startAdr = listAdr;

	if (elementMarker == HashNode::FREE) return;

	// Probing solution since there is a collision here
	while (elementMarker != HashNode::FREE && (elementMarker == HashNode::TOMBSTONE || !(listAdr->pair.first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) // we finished a cycle and the element doesn't exist.
			return; 
	}

	if (elementMarker == HashNode::OCCUPIED) { // We found the element
		m_UsedBuckets--;
		Destroy(&listAdr->pair.key); // listAdr->~HashNode();
		Destroy(&listAdr->pair.value);
		listAdr->tag = HashNode::TOMBSTONE; // Put Tombstone as its deleted
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::ContainsKey(const K& key) const
{
	V* ptr = this->GetKeyPtr(key);
	return (bool)(ptr);
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Clear()
{
	for (usize i = 0; i < m_Capacity; i++) {
		HashTab_t listAdr = m_HashTable + i;
		uint8 elementMarker = listAdr->tag;

		if (elementMarker == HashNode::OCCUPIED) {
			Destroy(&listAdr->pair.key); // listAdr->~HashNode();
			Destroy(&listAdr->pair.value);
		}

		listAdr->tag = HashNode::FREE;
	}
	
	m_UsedBuckets = 0;
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::IsEmpty() const
{
	return m_UsedBuckets == 0;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Resize(usize newSize)
{
	if (newSize < m_Capacity) 
		return;

	m_UsedBuckets = 0;
	HashTab_t newAdr = Allocate<HashNode>(newSize);

	for (usize i = 0; i < newSize; i++) {
		newAdr[i].tag = HashNode::FREE;
	}

	usize oldCapacity = m_Capacity;
	m_Capacity = newSize;

	for (usize i = 0; i < oldCapacity; i++) {
		HashTab_t srcAdr = m_HashTable + i;
		if (srcAdr->tag == HashNode::OCCUPIED) {
			this->Reinsert(srcAdr, newAdr);
		}
	}

	Free(m_HashTable);
	m_HashTable = newAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::CalculateAdress(const K& key)
{
	//if (m_UsedBuckets >= (usize) (m_Capacity * DEFAULT_LOAD_FACTOR))
	//	this->Resize(m_Capacity + S * 2);

	HashTab_t listAdr = this->InternalListCheck(key); // Should handle probing

	// Resize and insert in the right place while there is no place left.
	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->InternalListCheck(key);		// Should handle probing
	}

	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::Reinsert(HashTab_t src, HashTab_t dest)
{
	uint32 x = 0;
	K& key = (src->pair.first);
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = dest + initialIndex;
	HashTab_t startAdr = listAdr;
	uint8 elementMarker = listAdr->tag;

	// Probing solution since there is a collision here
	while (elementMarker && !(listAdr->pair.first == key)) {
		listAdr = dest + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL;
	}

	m_UsedBuckets++;
	MoveRangeTo(src, listAdr, 1);
	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::InternalListCheck(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);

	if (!m_HashTable) {
		m_HashTable = Allocate<HashNode>(m_Capacity);

		for (usize i = 0; i < m_Capacity; i++) {
			m_HashTable[i].tag = HashNode::NodeTag::FREE;
		}

		m_UsedBuckets++;
		HashTab_t listAdr = m_HashTable + initialIndex; // Its safe since the table is empty
		listAdr->tag = HashNode::OCCUPIED;
		return listAdr;
	}

	HashTab_t listAdr = m_HashTable + initialIndex;
	HashTab_t startAdr = listAdr;
	uint8 elementMarker = listAdr->tag;

	// Probing solution since there is a collision here
	while (elementMarker == HashNode::OCCUPIED && !(listAdr->pair.first == key)) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL; 
	}

	if (elementMarker == HashNode::OCCUPIED) { // Means directly they have same key!
		Destroy(&listAdr->pair.key); // listAdr->~HashNode();
		Destroy(&listAdr->pair.value);
		return listAdr;
	}else if (elementMarker == HashNode::TOMBSTONE) { // listAdr is the tombstone adress that we will return
		listAdr->tag = HashNode::OCCUPIED;
		m_UsedBuckets++;
		return listAdr; 
	}

	m_UsedBuckets++;
	listAdr->tag = HashNode::OCCUPIED; // listAdr is a free slot
	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::GetOrEmplace(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);

	if (!m_HashTable) {
		m_HashTable = Allocate<HashNode>(m_Capacity);

		for (usize i = 0; i < m_Capacity; i++) {
			m_HashTable[i].tag = HashNode::NodeTag::FREE;
		}

		m_UsedBuckets++;
		HashTab_t listAdr = m_HashTable + initialIndex; // Its safe since the table is empty
		new (&listAdr->pair) HashPair(key, V());
		listAdr->tag = HashNode::OCCUPIED;
		return listAdr;
	}

	HashTab_t listAdr = m_HashTable + initialIndex;
	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	uint8 elementMarker = listAdr->tag;

	// Probing solution since there is a collision here
	while (elementMarker != HashNode::FREE && (elementMarker == HashNode::TOMBSTONE || !(listAdr->pair.first == key))) {
		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && tombstoneAdr == NULL) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = listAdr->tag;

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL;
	}

	if (elementMarker == HashNode::OCCUPIED) {
		if (tombstoneAdr) {	// Seen tombstone already
			MoveRangeTo(listAdr, tombstoneAdr, 1); // perform a copy, should handle copying the tag
			listAdr->tag = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return listAdr;
	}else if (tombstoneAdr) {
		m_UsedBuckets++;
		new (&tombstoneAdr->pair) HashPair(key, V());
		tombstoneAdr->tag = HashNode::OCCUPIED;
		return tombstoneAdr; // listAdr is the tombstone adress that we will return
	}

	// Free slot
	m_UsedBuckets++;
	new (&listAdr->pair) HashPair(key, V());
	listAdr->tag = HashNode::OCCUPIED;
	return listAdr; // listAdr is the tombstone adress that we will return
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateIndex(const usize hash, const uint32 x) const
{
	return ((hash + Probe(x)) % m_Capacity);
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateHash(const K& key) const
{
	return Hash::GetHash(key);
}

template<typename K, typename V, usize S>
template<typename Ki, typename Vi, typename ::std::enable_if<HAVE_DTOR(Ki) || HAVE_DTOR(Vi), int>::type>
FORCEINLINE void HashMap<K, V, PROBING, S>::DestroyMap(HashMap<Ki, Vi, PROBING, S>& map)
{
    for (usize i = 0; i < map.m_Capacity; i++) {
		HashTab_t listAdr = map.m_HashTable + i;
		uint8 elementMarker = listAdr->tag;

		if (elementMarker == HashNode::OCCUPIED) {
			Destroy(&listAdr->pair.key); // listAdr->~HashNode();
			Destroy(&listAdr->pair.value);
		}
	}
}

template<typename K, typename V, usize S>
template<typename Ki, typename Vi, typename ::std::enable_if<NO_DTOR(Ki) && NO_DTOR(Vi), int>::type>
FORCEINLINE void HashMap<K, V, PROBING, S>::DestroyMap(HashMap<Ki, Vi, PROBING, S>& map)
{
    // DO NOTHING
}

////////////////////////////////////// ITERATORS //////////////////////////////////////

//TODO : This should be fixed since there is holes in the array ( I think its done)
template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::begin() noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return Iterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	uint8 elementMarker = adr->tag;

	while (!(elementMarker == HashNode::OCCUPIED) && i < m_Capacity) {
		adr = m_HashTable + ++i;
		elementMarker = adr->tag;
	}

	return Iterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::end() noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return Iterator(this, m_HashTable);

	return Iterator(this, NULL);
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::begin() const noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return Iterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = adr->tag;

	while (!(elementMarker == HashNode::OCCUPIED) && i < m_Capacity) {
		adr = m_HashTable + ++i;
		elementMarker = adr->tag;
	}

	return Iterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::end() const noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return Iterator(this, m_HashTable);

	return Iterator(this, NULL);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::CIterator HashMap<K, V, PROBING, S>::cbegin() const noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return CIterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = adr->tag;

	while (!(elementMarker == HashNode::OCCUPIED) && i < m_Capacity) {
		adr = m_HashTable + ++i;
		elementMarker = adr->tag;
	}

	return CIterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::CIterator HashMap<K, V, PROBING, S>::cend() const noexcept
{
	if (m_UsedBuckets == 0 || !m_HashTable)
		return CIterator(this, m_HashTable);

	return CIterator(this, NULL);
}

TRE_NS_END